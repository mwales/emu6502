#include"MemoryDev.h"
#include "Logger.h"

MemoryDev::MemoryDev(uint16_t address):
   theAddress(address)
{
   theSize = 0;
   theName = "Memory";
}

std::string MemoryDev::getName()
{
   return theName;
}

uint16_t MemoryDev::getAddress()
{
   return theAddress;
}

bool MemoryDev::isAbsAddressValid(uint16_t addr)
{
   if (addr < theAddress)
   {
      LOG_FATAL() << "ROM - Address " << addr << " is below the start address of ROM " << theName;
      return false;
   }

   if (addr > (theAddress + theSize))
   {
      LOG_FATAL() << "ROM - Address " << addr << " is after the end address of ROM " << theName;
      return false;
   }

   return true;
}
