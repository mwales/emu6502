#include "MirrorMemory.h"
#include <string.h>

#include "Logger.h"
#include "MemoryController.h"

#define MIRROR_MEM_TRACE

#ifdef MIRROR_MEM_TRACE
   #define MIRROR_DEBUG    LOG_DEBUG
   #define MIRROR_WARNING  LOG_WARNING
#else
   #define MIRROR_DEBUG    if(0) LOG_DEBUG
   #define MIRROR_WARNING  if(0) LOG_WARNING
#endif

// Static methods
std::string MirrorMemory::getTypeName()
{
   return "MirrorMemory";
}

MemoryDev* mirrorMemoryDeviceConstructor(std::string name)
{
   return new MirrorMemory(name);
}

MemoryDeviceConstructor MirrorMemory::getMDC()
{
   return mirrorMemoryDeviceConstructor;
}

MirrorMemory::MirrorMemory(std::string name):
   MemoryDev(name),
   theConfigFlags(0),
   theSizeOfMemoryMirrored(0),
   theAddrOfMemoryMirrored(0),
   theRealMemDev(nullptr)
{
   MIRROR_DEBUG() << "Created a Mirror Memory device: " << name;
}

MirrorMemory::~MirrorMemory()
{
   // empty
}

uint8_t MirrorMemory::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      return 0;
   }

   CpuAddress offset = absAddr - theAddress;
   CpuAddress mask = theSizeOfMemoryMirrored - 1;
   CpuAddress original = (offset & mask) + theAddrOfMemoryMirrored;

   MIRROR_DEBUG() << "read8 at " << addressToString(absAddr) << " reading "
                  << addressToString(original);
   return theRealMemDev->read8(original);
}

bool MirrorMemory::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   CpuAddress offset = absAddr - theAddress;
   CpuAddress mask = theSizeOfMemoryMirrored - 1;
   CpuAddress original = (offset & mask) + theAddrOfMemoryMirrored;

   MIRROR_DEBUG() << "write8 at " << addressToString(absAddr) << " writing "
                  << addressToString(original);
   return theRealMemDev->write8(original, val);
}

uint16_t MirrorMemory::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   CpuAddress offset = absAddr - theAddress;
   CpuAddress mask = theSizeOfMemoryMirrored - 1;
   CpuAddress original = (offset & mask) + theAddrOfMemoryMirrored;

   MIRROR_DEBUG() << "read16 at " << addressToString(absAddr) << " reading "
                  << addressToString(original);
   return theRealMemDev->read16(original);
}

bool MirrorMemory::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   CpuAddress offset = absAddr - theAddress;
   CpuAddress mask = theSizeOfMemoryMirrored - 1;
   CpuAddress original = (offset & mask) + theAddrOfMemoryMirrored;

   MIRROR_DEBUG() << "write16 at " << addressToString(absAddr) << " writing "
                  << addressToString(original);
   return theRealMemDev->write16(original, val);
}

// RAM configuration flags
#define MIRROR_ADDR_CONFIG   0x01
#define MIRROR_SIZE_CONFIG   0x02
#define ORIGINAL_ADDR_CONFIG 0x04
#define MIRROR_CONFIG_DONE (MIRROR_ADDR_CONFIG | MIRROR_SIZE_CONFIG | ORIGINAL_ADDR_CONFIG)

bool MirrorMemory::isFullyConfigured()
{
   return (theConfigFlags == MIRROR_CONFIG_DONE);
}

std::vector<std::string> MirrorMemory::getIntConfigParams()
{
   std::vector<std::string> retVal;
   retVal.push_back("startAddress");
   retVal.push_back("size");
   retVal.push_back("originalMemoryAddress");
   return retVal;
}

std::vector<std::string> MirrorMemory::getStringConfigParams()
{
   std::vector<std::string> retVal;
   return retVal;
}

void MirrorMemory::setIntConfigValue(std::string paramName, int value)
{
   if (paramName == "startAddress")
   {
      theAddress = value;
      theConfigFlags |= MIRROR_ADDR_CONFIG;
   }

   if (paramName == "size")
   {
      theSize = value;
      theConfigFlags |= MIRROR_SIZE_CONFIG;
   }

   if (paramName == "originalMemoryAddress")
   {
      theAddrOfMemoryMirrored = value;
      theConfigFlags |= ORIGINAL_ADDR_CONFIG;
   }

}

void MirrorMemory::setStringConfigValue(std::string paramName, std::string value)
{
   // Empty
}

void MirrorMemory::resetMemory()
{
   // If the object is completely configured, initialize itself properly
   if (isFullyConfigured())
   {
      // Get a pointer to the original memory device

      if (theMemController == nullptr)
      {
         LOG_FATAL() << "Memory controller not initialized, cant reset mirror memory";
         return;
      }

      theRealMemDev = theMemController->getDevice(theAddrOfMemoryMirrored);
      if (theRealMemDev == nullptr)
      {
         LOG_FATAL() << "Cant find memory device at" << addressToString(theAddrOfMemoryMirrored)
                     << "for mirroring, did you initialize it first in the configuration?";
         return;
      }

      theSizeOfMemoryMirrored = theRealMemDev->getSize();
      MIRROR_DEBUG() << "Mirror Memory " << theName << " mirroring memory " << theRealMemDev->getName()
                     << " from address" << addressToString(theAddrOfMemoryMirrored) << ", size = "
                     << theSizeOfMemoryMirrored << " bytes";

      if (theSize % theSizeOfMemoryMirrored)
      {
         MIRROR_WARNING() << "Mirror memory " << theName << "size isn't a multiple of original size";
      }

      MIRROR_DEBUG() << "MIRROR MEMORY INITIALIZED: " << theSize << " bytes "
                     << addressToString(theAddress) << "-"
                     << addressToString(theAddress + theSize - 1);
   }
   else
   {
      LOG_FATAL() << "Mirror Memory has incomplete configuration!";
   }
}
