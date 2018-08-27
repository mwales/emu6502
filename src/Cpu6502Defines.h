#ifndef CPU6502DEFINES_H
#define CPU6502DEFINES_H

#include "Utils.h"

typedef uint16_t CpuAddress;

typedef std::pair<CpuAddress, CpuAddress> MemoryRange;

inline std::string addressToString(CpuAddress val)
{
   return Utils::toHex16(val);
}

class Decoder6502;

enum AddressMode6502
{
   IMPLIED,
   IMMEDIATE,
   RELATIVE,
   INDIRECT,
   INDIRECT_X,
   INDIRECT_Y,
   ABSOLUTE,
   ZERO_PAGE,
   ABSOLUTE_X,
   ABSOLUTE_Y,
   ZERO_PAGE_X,
   ZERO_PAGE_Y
};

// 6502 Status Flags
#define FLG_NONE 0x00
#define FLG_CARY 0x01
#define FLG_ZERO 0x02
#define FLG_INTD 0x04
#define FLG_DECI 0x08
#define FLG_BKPT 0x10
// #define FLG_  0x20
#define FLG_OVFL 0x40
#define FLG_NEG  0x80

typedef union StatusRegUnion
{
   uint8_t theWholeRegister;
   struct
   {
       uint8_t theCarryFlag:1;
       uint8_t theZeroFlag:1;
       uint8_t theInterruptFlag:1;
       uint8_t theBCDFlag:1;
       uint8_t theBreakpointFlag:1;
       uint8_t theNotUsedFlag:1;
       uint8_t theOverflowFlag:1;
       uint8_t theSignFlag:1;
    };
} StatusReg;


typedef struct OpCodeInfoStruct
{
   uint8_t theOpCode;
   uint8_t theNumBytes;
   uint8_t theStatusFlags;
   uint8_t theDelayCycles;

   char const * theMnemonicDisass;

   bool    theExtraDelay;

   enum AddressMode6502 theAddrMode;

   void (Decoder6502::*theOpCodeHandler)(struct OpCodeInfoStruct* info);

} OpCodeInfo;

void constructCpuGlobals();

extern OpCodeInfo gOpCodes[0xff];

#define CPU_OP(opCode, numBytes, disMnemonic, mnemonic, addrMode, cycles, extraCycles, flags) \
   gOpCodes[opCode].theOpCode = opCode; \
   gOpCodes[opCode].theNumBytes = numBytes; \
   gOpCodes[opCode].theStatusFlags = flags; \
   gOpCodes[opCode].theDelayCycles = cycles; \
   gOpCodes[opCode].theMnemonicDisass = #disMnemonic; \
   gOpCodes[opCode].theAddrMode = addrMode; \
   gOpCodes[opCode].theOpCodeHandler = &Decoder6502::handler_##mnemonic;




#endif // CPU6502DEFINES_H
