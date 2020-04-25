#ifndef EMULATOR_COMMON
#define EMULATOR_COMMON

#include <stdint.h>
#include <utility>
#include <iostream>
#include <stdio.h>

typedef uint32_t CpuAddress;

typedef std::pair<CpuAddress, CpuAddress> MemoryRange;

inline std::string addressToString(CpuAddress val)
{
   char buf[12];
   snprintf(buf, 12, "0x%08x", val);
   return std::string(buf);
}


#endif

