#include "RomMemory.h"
#include "EmulatorConfig.h"
#include "ConfigManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "Logger.h"

#ifdef ROM_TRACE
   #define ROM_DEBUG    LOG_DEBUG
   #define ROM_WARNING  LOG_WARNING
#else
   #define ROM_DEBUG    if(0) LOG_DEBUG
   #define ROM_WARNING  if(0) LOG_WARNING
#endif

// Static methods
std::string RomMemory::getTypeName()
{
   return "ROM";
}

MemoryDev* romDeviceConstructor(std::string name)
{
   return new RomMemory(name);
}

MemoryDeviceConstructor RomMemory::getMDC()
{
   return romDeviceConstructor;
}

RomMemory::RomMemory(std::string name):
   MemoryDev(name),
   theData(nullptr),
   theRomFile(""),
   theConfigFlags(0),
   theStartEmulationAddressSetFlag(false),
   theStartEmulationAddress(0)
{
   ROM_DEBUG() << "Created a ROM device: " << name;
   
   theUint16ConfigParams.emplace("startAddress", &theAddress);
   theStrConfigParams.emplace("filename", &theRomFile);
   // startEmulatorAddress also implemented in the configSelf method
}

RomMemory::~RomMemory()
{
   if (theData)
   {
      delete[] theData;
      theData = 0;
   }
}

uint8_t RomMemory::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      return 0;
   }

   return theData[absAddr - theAddress];
}

bool RomMemory::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   theData[absAddr - theAddress] = val;
   return true;
}

uint16_t RomMemory::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   uint16_t* retData = (uint16_t*) &theData[absAddr - theAddress];
   return *retData;
}

bool RomMemory::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   uint16_t* dataPtr = (uint16_t*) &theData[absAddr - theAddress];
   *dataPtr = val;
   return true;
}

// ROM configuration flags
#define ROM_ADDR_CONFIG 0x01
#define ROM_FILE_CONFIG 0x02
#define ROM_CONFIG_DONE (ROM_ADDR_CONFIG | ROM_FILE_CONFIG)

bool RomMemory::isFullyConfigured() const
{
   return (theConfigFlags == ROM_CONFIG_DONE);
}

std::string RomMemory::getConfigTypeName() const
{
   return getTypeName();
}

std::vector<std::string> RomMemory::getIntConfigParams() const
{
   std::vector<std::string> retVal;
   retVal.push_back("startAddress");
   return retVal;
}

std::vector<std::string> RomMemory::getStringConfigParams() const
{
   std::vector<std::string> retVal;
   retVal.push_back("romFilename");
   return retVal;
}

void RomMemory::setIntConfigValue(std::string paramName, int value)
{
   ROM_DEBUG() << "setIntConfigValue(" << paramName << "," << value << ")";

   if (paramName == "startAddress")
   {
      theAddress = value;
      theConfigFlags |= ROM_ADDR_CONFIG;
      ROM_DEBUG() << "ROM " << theName << " startAddress = " << addressToString(theAddress);
   }
}

void RomMemory::setStringConfigValue(std::string paramName, std::string value)
{
   ROM_DEBUG() << "setStringConfigValue(" << paramName << "," << value << ")";

   if (paramName == "romFilename")
   {
      theRomFile = value;
      theConfigFlags |= ROM_FILE_CONFIG;
      ROM_DEBUG() << "ROM " << theName << " filename = " << theRomFile;
   }
}

void RomMemory::resetMemory()
{
   loadRomIntoMemory();
}

void RomMemory::loadRomIntoMemory()
{
//      if (!isFullyConfigured())
//      {
//         LOG_FATAL() << "ROM " << theName << " not fully configured during reset ("
//                     << this << ")";
//      }

      int fd = open(theRomFile.c_str(), O_RDONLY);

      if (fd <= 0)
      {
         ROM_WARNING() << "ROM INIT ERROR: Couldn't open ROM file " << theRomFile;
         return;
      }

      int32_t sizeStatus =  lseek(fd, 0, SEEK_END);
      if (sizeStatus < 0)
      {
         ROM_WARNING() << "ROM INIT ERROR: Couldn't seek to the end of the ROM";
         close(fd);
         return;
      }

      if (sizeStatus > UINT16_MAX)
      {
         ROM_WARNING() << "ROM INIT ERROR: ROM File too large for 6502 memory space";
         close(fd);
         return;
      }

      if (sizeStatus > (UINT16_MAX - theAddress + 1))
      {
         ROM_WARNING() << "ROM INIT ERROR: ROM file will not fit in the memory region.  Space ="
                       << Utils::toHex16(UINT16_MAX - theAddress + 1) << ", ROM size = "
                       << Utils::toHex16(sizeStatus);
         close(fd);
         return;
      }

      theSize = sizeStatus;

      if (theData != nullptr)
      {
         delete[] theData;
      }

      theData = new uint8_t[theSize];

      lseek(fd, 0, SEEK_SET);

      uint16_t bytesLeft = theSize;
      int bytesReadCumulative = 0;
      while(bytesLeft)
      {
         int bytesRead = read(fd, &theData[bytesReadCumulative], bytesLeft);

         if (bytesRead <= 0)
         {
            ROM_WARNING() << "ROM INIT ERROR: Error reading the ROM contents";
            close(fd);
            return;
         }

         bytesLeft -= bytesRead;
         bytesReadCumulative += bytesRead;
      }

      ROM_DEBUG() << "ROM INITIALIZED: " << theRomFile << " (" << theSize << " bytes) "
                  << addressToString(theAddress) << "-" << addressToString(theAddress + theSize -1);
      close(fd);
}

bool RomMemory::specifiesStartAddress() const
{
   return theStartEmulationAddressSetFlag;
}

CpuAddress RomMemory::getStartPcAddress() const
{
   return theStartEmulationAddress;
}

bool RomMemory::configSelf()
{
   // Is startEmulatorAddress config set for this instance?
   ConfigManager* configMgr = ConfigManager::getInstance();
   if (configMgr->isConfigPresent(getConfigTypeName(), theName, "startEmulatorAddress"))
   {
      uint32_t pc = configMgr->getIntegerConfigValue(getConfigTypeName(), theName, "startEmulatorAddress");
      theStartEmulationAddress = (CpuAddress) pc;

      ROM_DEBUG() << "ROM config specificies emulation start address:" << addressToString(theStartEmulationAddress);
   }

   bool retVal = MemoryDev::configSelf();

   if (retVal)
   {
      loadRomIntoMemory();
   }

   return retVal;
}
