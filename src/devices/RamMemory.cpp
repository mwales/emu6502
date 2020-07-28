#include <string.h>

#include "RamMemory.h"
#include "EmulatorConfig.h"
#include "Logger.h"
#include "MemoryFactory.h"
#include "force_execute.h"
#include "Utils.h"
#include "ConfigManager.h"

#ifdef RAM_TRACE
   #define RAM_DEBUG    LOG_DEBUG
   #define RAM_WARNING  LOG_WARNING
#else
   #define RAM_DEBUG    if(0) LOG_DEBUG
   #define RAM_WARNING  if(0) LOG_WARNING
#endif

// Static methods
std::string RamMemory::getTypeName()
{
   return "RAM";
}

MemoryDev* ramDeviceConstructor(std::string name)
{
   return new RamMemory(name);
}

MemoryDeviceConstructor RamMemory::getMDC()
{
   return ramDeviceConstructor;
}

RamMemory::RamMemory(std::string name):
   MemoryDev(name),
   theConfigFlags(0),
   theData(nullptr),
   theLoadDataOffset(0)
{
   RAM_DEBUG() << "Created a RAM device: " << name;

   theUint32ConfigParams.add("size", &theSize);
   theUint32ConfigParams.add("startAddress", &theAddress);
   theUint32ConfigParams.add("loadDataOffset", &theLoadDataOffset);
   theOptionalParams.add("loadDataOffset", false);
   theStrConfigParams.add("loadDataFile", &theLoadDataFile);
   theOptionalParams.add("loadDataFile", false);
}

RamMemory::~RamMemory()
{
   if (theData)
   {
      delete[] theData;
      theData = 0;
   }
}

uint8_t RamMemory::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      return 0;
   }

   return theData[absAddr - theAddress];
}

bool RamMemory::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   theData[absAddr - theAddress] = val;
   return true;
}

uint16_t RamMemory::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   uint16_t* retData = (uint16_t*) &theData[absAddr - theAddress];
   return *retData;
}

bool RamMemory::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   uint16_t* dataPtr = (uint16_t*) &theData[absAddr - theAddress];
   *dataPtr = val;
   return true;
}

// RAM configuration flags
#define RAM_ADDR_CONFIG 0x01
#define RAM_SIZE_CONFIG 0x02
#define RAM_CONFIG_DONE (RAM_ADDR_CONFIG | RAM_SIZE_CONFIG)

bool RamMemory::isFullyConfigured() const
{
   return (theConfigFlags == RAM_CONFIG_DONE);
}

std::string RamMemory::getConfigTypeName() const
{
   return getTypeName();
}

void RamMemory::resetMemory()
{
   // If the object is completely configured, initialize itself properly
   // if (isFullyConfigured())
   {
      if (theData != nullptr)
      {
         RAM_WARNING() << "Reconfiguring RAM that was already initialized once";
         delete[] theData;
      }

      theData = new uint8_t[theSize];
      memset(theData, 0, theSize);

      RAM_DEBUG() << "RAM INITIALIZED: " << theSize << " bytes "
                  << addressToString(theAddress) << "-"
                  << addressToString(theAddress + theSize - 1);
   } 
   
   if (!theLoadDataFile.empty())
   {
      // There is a file with initial RAM state to load
      std::string configStr = ConfigManager::getInstance()->getConfigDirectory();

      std::string fullPath = configStr + "/" + theLoadDataFile;

      RAM_DEBUG() << "Going to load initial RAM data from" << fullPath << " at " << addressToString(theLoadDataOffset);
      std::string errorStr;
      std::vector<uint8_t> data = Utils::loadFileBytes(fullPath, errorStr);
      
      if (!errorStr.empty())
      {
         RAM_WARNING() << "Error loading RAM data from file: " << fullPath
                       << ": " << errorStr;
         return;
      }
      
      // copy the  file contents into RAM
      CpuAddress dataWriteLoc = theLoadDataOffset;
      for(auto curByte: data)
      {
         if (dataWriteLoc >= theSize)
         {
            RAM_WARNING() << "Error loading RAM data, out of RAM";
            return;
         }
         
         theData[dataWriteLoc] = curByte;
         dataWriteLoc += 1;
      }

      RAM_DEBUG() << "Data Loaded from file:" << Utils::hexDump(data.data(), data.size());
   }
   else
   {
      RAM_DEBUG() << "No filename for data to load into RAM";
   }
}

FORCE_EXECUTE(fe_ram_memory)
{
	MemoryFactory* mf = MemoryFactory::getInstance();
	mf->registerMemoryDeviceType(RamMemory::getTypeName(), RamMemory::getMDC());
}

