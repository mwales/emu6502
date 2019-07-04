#include <string.h>

#include "RngDev.h"
#include "EmulatorConfig.h"
#include "Logger.h"

#include <stdlib.h>
#include <time.h>

#ifdef RANDOMDEV_TRACE
   #define RNG_DEBUG    LOG_DEBUG
   #define RNG_WARNING  LOG_WARNING
#else
   #define RNG_DEBUG    if(0) LOG_DEBUG
   #define RNG_WARNING  if(0) LOG_WARNING
#endif

// Static methods
std::string RngDev::getTypeName()
{
   return "RngDev";
}

MemoryDev* rngDeviceConstructor(std::string name)
{
   return new RngDev(name);
}

MemoryDeviceConstructor RngDev::getMDC()
{
   return rngDeviceConstructor;
}

RngDev::RngDev(std::string name):
   MemoryDev(name),
   theConfigFlags(0)
{
   int timeSeed = time(0);
   srand(timeSeed);

   RNG_DEBUG() << "Created a random number generator device: " << name;
   RNG_DEBUG() << "RNG Seed = " << timeSeed;
}

RngDev::~RngDev()
{
   RNG_DEBUG() << "RNG device @ " << addressToString(theAddress) << " being deleted";
}

uint8_t RngDev::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      return 0;
   }

   return rand() & 0xff;
}

bool RngDev::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   RNG_WARNING() << "RNG write @ " << addressToString(absAddr);
   return true;
}

uint16_t RngDev::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   return rand() & 0xffff;
}

bool RngDev::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   RNG_WARNING() << "RNG write @ " << addressToString(absAddr);

   return true;
}

// RNG configuration flags
#define RNG_ADDR_CONFIG 0x01
#define RNG_SIZE_CONFIG 0x02
#define RNG_CONFIG_DONE (RNG_ADDR_CONFIG | RNG_SIZE_CONFIG)

bool RngDev::isFullyConfigured()
{
   return (theConfigFlags == RNG_CONFIG_DONE);
}

std::vector<std::string> RngDev::getIntConfigParams()
{
   std::vector<std::string> retVal;
   retVal.push_back("startAddress");
   retVal.push_back("size");
   return retVal;
}

std::vector<std::string> RngDev::getStringConfigParams()
{
   std::vector<std::string> retVal;
   return retVal;
}

void RngDev::setIntConfigValue(std::string paramName, int value)
{
   if (paramName == "startAddress")
   {
      theAddress = value;
      theConfigFlags |= RNG_ADDR_CONFIG;
   }

   if (paramName == "size")
   {
      theSize = value;
      theConfigFlags |= RNG_SIZE_CONFIG;
   }
}

void RngDev::setStringConfigValue(std::string paramName, std::string value)
{
   // Purposely empty
}

void RngDev::resetMemory()
{
   // If the object is completely configured, initialize itself properly
   if (isFullyConfigured())
   {
      LOG_DEBUG() << "RNGDEV INITIALIZED: " << theSize << " bytes "
                  << addressToString(theAddress) << "-"
                  << addressToString(theAddress + theSize - 1);
   }
}
