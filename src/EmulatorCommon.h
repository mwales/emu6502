#ifndef EMULATOR_COMMON
#define EMULATOR_COMMON

#include <cstdint>
#include <utility>
#include <iostream>
#include <cstdio>
#include "Utils.h"

typedef uint32_t CpuAddress;

typedef std::pair<CpuAddress, CpuAddress> MemoryRange;

inline std::string addressToString(CpuAddress val)
{
   char buf[12];
   snprintf(buf, 12, "0x%08x", val);
   return std::string(buf);
}

inline bool stringToAddress(std::string userInput, CpuAddress* addr)
{
   bool retVal;
   CpuAddress data = Utils::parseUInt32(userInput, &retVal);
   
   if (retVal)
   {
      *addr = data;
   }
   
   return retVal;
}

#endif

