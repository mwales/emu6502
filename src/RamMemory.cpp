#include "RamMemory.h"
#include <string.h>

#include "Logger.h"

RamMemory::RamMemory(CpuAddress address, uint16_t size):
   theData(0)
{
   theAddress = address;
   theName = "RAM";
   theSize = size;

   theData = new uint8_t[theSize];

   memset(theData, 0, theSize);

   LOG_DEBUG() << "RAM INITIALIZED: " << theSize << " bytes "
               << addressToString(theAddress) << "-" << addressToString(theAddress + theSize);
}

RamMemory::~RamMemory()
{
   if (theData)
   {
      delete[] theData;
      theData = 0;
   }
}

void RamMemory::setName(std::string name)
{
   theName = name;
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


