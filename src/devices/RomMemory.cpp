#include "RomMemory.h"
#include "EmulatorConfig.h"
#include "ConfigManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "force_execute.h"
#include "MemoryFactory.h"
#include "Utils.h"


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
   
   theUint32ConfigParams.add("startAddress", &theAddress);
   theStrConfigParams.add("filename", &theRomFile);
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

   std::string absRomFilePath = ConfigManager::getInstance()->getConfigDirectory();
   absRomFilePath += theRomFile;
      int fd = open(absRomFilePath.c_str(), O_RDONLY);

      if (fd <= 0)
      {
         ROM_WARNING() << "ROM INIT ERROR: Couldn't open ROM file " << theRomFile;
         return;
      }

      int64_t sizeStatus =  lseek(fd, 0, SEEK_END);
      if (sizeStatus < 0)
      {
         ROM_WARNING() << "ROM INIT ERROR: Couldn't seek to the end of the ROM";
         close(fd);
         return;
      }

      if (sizeStatus > (UINT32_MAX - theAddress + 1U))
      {
         ROM_WARNING() << "ROM INIT ERROR: ROM file will not fit in the memory region.  Space ="
                       << Utils::toHex32(UINT32_MAX - theAddress + 1) << ", ROM size = "
                       << Utils::toHex32(sizeStatus);
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
      theStartEmulationAddressSetFlag = true;

      ROM_DEBUG() << "ROM config specificies emulation start address:" << addressToString(theStartEmulationAddress);
   }

   bool retVal = MemoryDev::configSelf();

   if (retVal)
   {
      loadRomIntoMemory();
   }

   return retVal;
}

FORCE_EXECUTE(fe_rom_memory)
{
	MemoryFactory* mf = MemoryFactory::getInstance();
	mf->registerMemoryDeviceType(RomMemory::getTypeName(), RomMemory::getMDC());
}
