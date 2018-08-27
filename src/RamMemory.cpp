#include "RamMemory.h"
#include <string.h>

#include "Logger.h"

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
   theData(nullptr)
{
   RAM_DEBUG() << "Created a RAM device: " << name;
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

bool RamMemory::isFullyConfigured()
{
   return (theConfigFlags == RAM_CONFIG_DONE);
}

std::vector<std::string> RamMemory::getIntConfigParams()
{
   std::vector<std::string> retVal;
   retVal.push_back("startAddress");
   retVal.push_back("size");
   return retVal;
}

std::vector<std::string> RamMemory::getStringConfigParams()
{
   std::vector<std::string> retVal;
   return retVal;
}

void RamMemory::setIntConfigValue(std::string paramName, int value)
{
   if (paramName == "startAddress")
   {
      theAddress = value;
      theConfigFlags |= RAM_ADDR_CONFIG;
   }

   if (paramName == "size")
   {
      theSize = value;
      theConfigFlags |= RAM_SIZE_CONFIG;
   }
}

void RamMemory::setStringConfigValue(std::string paramName, std::string value)
{
   // Purposely empty
}

void RamMemory::resetMemory()
{
   // If the object is completely configured, initialize itself properly
   if (isFullyConfigured())
   {
      if (theData != nullptr)
      {
         LOG_WARNING() << "Reconfiguring RAM that was already initialized once";
         delete[] theData;
      }

      theData = new uint8_t[theSize];
      memset(theData, 0, theSize);

      LOG_DEBUG() << "RAM INITIALIZED: " << theSize << " bytes "
                  << addressToString(theAddress) << "-"
                  << addressToString(theAddress + theSize - 1);
   }
}
