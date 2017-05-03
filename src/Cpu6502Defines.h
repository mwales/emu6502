#ifndef CPU6502DEFINES_H
#define CPU6502DEFINES_H

#include "Utils.h"

typedef uint16_t CpuAddress;

inline std::string addressToString(CpuAddress val)
{
   return Utils::toHex16(val);
}

enum class OpCodeAddressMode
{
   IMMEDIATE = 0,       // 2 byte instruction  MNM #$xx
   ABSOLUTE,            // 3 byte instruction  MNM $xxxx
   ABSOLUTE_ZERO_PAGE,  // 2 byte instruction  MNM $xx
   IMPLIED,             // 1 byte instruction  MNM
   INDEXED,             // 3 byte instruction  MNM $xxxx, X
   INDEXED_ZERO_PAGE,   // 2 byte instruction  MNM $xx, X
   INDIRECT,            // 3 byte instruction  MNM ($xxxx)
   RELATIVE,            // 2 byte instruction  MNM *+4
   INDIRECT_INDEXED,    // 2 byte instruction  MNM ($xx, Y)
   INDEXED_INDIRECT,    // 2 byte instruction  MNM ($xx), Y
   INVALID
};

enum class OpCode6502
{
   BRK                       = 0x00,
   ORA_X_INDIRECT_INDEXED,  // 0x01
   // 0x02
   // 0x03
   // 0x04
   ORA_ABSOLUTE_ZEROPAGE     = 0x05,
   ASL_ABSOLUTE_ZEROPAGE,   // 0x06
   // 0x07
   PUSH_PROC_STATUS          = 0x08,
   ORA_IMMEDIATE,           // 0x09
   ASL_A_IMPLIED,           // 0x0a
   // 0x0b
   // 0x0c
   ORA_ABSOLUTE              = 0x0d,
   ASL_ABSOLUTE,            // 0x0e,
   // 0xff

   BPL_REL                   = 0x10,
   ORA_Y_INDEXED_INDIRECT,  // 0x11
   // 0x12
   // 0x13
   // 0x14
   ORA_X_ABSOLUTE_ZERO_PAGE  = 0x15,
   ASL_X_ABSOLUTE_ZERO_PAGE,// 0x16
   // 0x17
   CLC                       = 0x18,
   ORA_Y_ABSOLUTE,          // 0x19
   // 0x1a
   // 0x1b
   // 0x1c
   ORA_X_ABSOLUTE            = 0x1d,
   ASL_X_ABSOLUTE,          // 0x1e
   // 0x1f

   JSR_ABSOLUTE              = 0x20,
   AND_X_INDIRECT_INDEXED,  // 0x21
   // 0x22
   // 0x23
   BIT_ABSOLUTE_ZERO_PAGE    = 0x24,
   AND_ABSOLUTE_ZERO_PAGE,  // 0x25
   ROL_ABSOLUTE_ZERO_PAGE,  // 0x26
   // 0x27
   PLP                       = 0x28,
   AND_IMMEDIATE,           // 0x29
   ROL_A_IMPLIED,           // 0x2a
   // 0x2b
   BIT_ABSOLUTE              = 0x2c,
   AND_ABSOLUTE,            // 0x2d
   ROL_ABSOLUTE,            // 0x2e
   // 0x2f

   BMI_REL                   = 0x30,
   AND_Y_INDEXED_INDIRECT,  // 0x31
   // 0x32
   // 0x33
   // 0x34
   AND_X_ABSOLUTE_ZERO_PAGE  = 0x35,
   ROL_X_ABSOLUTE_ZERO_PAGE,// 0x36
   // 0x37
   SEC                       = 0x38,
   AND_Y_ABSOLUTE,          // 0x39
   // 0x3a
   // 0x3b
   // 0x3c
   AND_X_ABSOLUTE            = 0x3d,
   ROL_X_ABSOLUTE,          // 0x3e
   // 0x3f

   RTI                       = 0x40,
   EOR_X_INDIRECT_INDEXED,  // 0x41
   // 0x42
   // 0x43
   // 0x44
   EOR_ABSOLUTE_ZERO_PAGE,  // 0x45
   LSR_ABSOLUTE_ZERO_PAGE,  // 0x46
   // 0x47
   PHA                       = 0x48,
   EOR_IMMEDIATE ,          // 0x49
   LSR_A,                   // 0x4a
   // 0x4b
   JMP_ABSOLUTE              = 0x4c,
   EOR_ABSOLUTE,            // 0x4d
   LSR_ABSOLUTE,            // 0x4e
   // 0x4f

   BVC_REL                   = 0x50,
   EOR_Y_INDEXED_INDIRECT,  // 0x51
   // 0x52
   // 0x53
   // 0x54
   EOR_X_ABSOLUTE_ZERO_PAGE  = 0x55,
   LSR_X_ABSOLUTE_ZERO_PAGE,// 0x56
   // 0x57
   CLI                       = 0x58,
   EOR_Y_ABSOLUTE,          // 0x59
   // 0x5a
   // 0x5b
   // 0x5c
   EOR_X_ABSOLUTE            = 0x5d,
   LSR_X_ABSOLUTE,          // 0x5e
   // 0x5f


};

extern enum OpCodeAddressMode ADDRESS_MODE[];

extern int OP_CODE_LENGTH[];




#endif // CPU6502DEFINES_H
