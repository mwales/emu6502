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

enum class OpCodeAddressModeOldModes
{
   IMMEDIATE = 0,       // 2 byte instruction  MNM #$xx
   ABSOLUTE,            // 3 byte instruction  MNM $xxxx
   ABSOLUTE_ZERO_PAGE,  // 2 byte instruction  MNM $xx
   IMPLIED,             // 1 byte instruction  MNM
   INDEXED,             // 3 byte instruction  MNM $xxxx, X
   INDEXED_ZERO_PAGE,   // 2 byte instruction  MNM $xx, X
   INDIRECT,            // 3 byte instruction  MNM ($xxxx)
   RELATIVE,            // 2 byte instruction  MNM *+4
   INDEXED_INDIRECT,    // 2 byte instruction  MNM ($xx, X)
   INDIRECT_INDEXED,    // 2 byte instruction  MNM ($xx), Y
   INVALID
};



//extern enum OpCodeAddressMode ADDRESS_MODE[];

//extern int OP_CODE_LENGTH[];

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
   ABS_ZP_X,
   ABS_ZP_Y,
   ZERO_PAGE_X,
   ZERO_PAGE_Y
};

// 6502 Status Flags
#define FLG_NONE 0x00
#define FLG_CARY 0x01
#define FLG_ZERO 0x02
#define FLG_DECI 0x04
#define FLG_INTD 0x08
// #define FLG_  0x10
#define FLG_OVFL 0x20
#define FLG_NEG  0x40


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
