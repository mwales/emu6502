#ifndef CPU6502DEFINES_H
#define CPU6502DEFINES_H

#include "Utils.h"

typedef uint16_t CpuAddress;

inline std::string addressToString(CpuAddress val)
{
   return Utils::toHex16(val);
}

enum class OpCode6502
{
   BRK                       = 0x00,
   ORA_X_INDIRECT_INDEXED,  // 0x01
   // 0x02
   // 0x04
   ORA_ABSOLUTE_ZEROPAGE     = 0x05,
   ASL_ABSOLUTE_ZEROPAGE,   // 0x06
   PUSH_PROC_STATUS          = 0x08,
   ORA_IMMEDIATE,           // 0x09
   ASL_A_IMPLIED,           // 0x0a
   // 0x0c
   ORA_ABSOLUTE              = 0x0d,
   ASL_ABSOLUTE,            // 0x0e,





   BPL_REL

};

enum class OpCodeAddressMode
{
   IMMEDIATE,           // 2 byte instruction   MNM #$xx
   ABSOLUTE,            // 3 byte instruction   MNM $xxxx
   ABSOLUTE_ZERO_PAGE,  // 2 byte instruction  MNM $xx
   IMPLIED,             // 1 byte instruction   MNM
   INDEXED,             // 3 byte instruction   MNM Y, $xxxx
   INDEXED_ZERO_PAGE,   // 2 byte instruction  MNM Y, $xx
   INDIRECT,            // 3 byte instruction  MNM ($xxxx)
   RELATIVE,            // 2 byte instruction  MNM *+4
   INDIRECT_INDEXED,    // MNM (Y, $xxxx)
   INDEXED_INDIRECT     // MNM ($xxxx), Y

};

#endif // CPU6502DEFINES_H
