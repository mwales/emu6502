#include"MemoryDev.h"
#include "Logger.h"
#include "Utils.h"

std::string MemoryDev::getName()
{
   return theName;
}

CpuAddress MemoryDev::getAddress()
{
   return theAddress;
}

CpuAddress MemoryDev::getSize()
{
   return theSize;
}

bool MemoryDev::isAbsAddressValid(CpuAddress addr, bool haltOnError)
{
   if (addr < theAddress)
   {
      LOG_FATAL() << "Memory (" << theName << ") - Address " << addressToString(addr) << " is below the start address of ROM " << theName;
      return false;
   }

   if (addr > (theAddress + theSize))
   {
      LOG_FATAL() << "Memory (" << theName << ") - Address " << addressToString(addr) << " is after the end address of ROM " << theName;
      return false;
   }

   return true;
}
