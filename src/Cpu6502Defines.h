#ifndef CPU6502DEFINES_H
#define CPU6502DEFINES_H

#include "Utils.h"

typedef uint16_t CpuAddress;

inline std::string addressToString(CpuAddress val)
{
   return Utils::toHex16(val);
}

#endif // CPU6502DEFINES_H
