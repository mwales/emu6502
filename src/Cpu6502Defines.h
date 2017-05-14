#ifndef CPU6502DEFINES_H
#define CPU6502DEFINES_H

#include "Utils.h"

typedef uint16_t CpuAddress;

typedef std::pair<CpuAddress, CpuAddress> MemoryRange;

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
   INDEXED_INDIRECT,    // 2 byte instruction  MNM ($xx, X)
   INDIRECT_INDEXED,    // 2 byte instruction  MNM ($xx), Y
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
   PHP                       = 0x08,
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
   ROL_A,                   // 0x2a
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
   EOR_ABSOLUTE_ZERO_PAGE    = 0x45,
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

   RTS                       = 0x60,
   ADC_X_INDIRECT_INDEXED,  // 0x61
   // 0x62
   // 0x63
   // 0x64
   ADC_ABSOLUTE_ZERO_PAGE    = 0x65,
   ROR_ABSOLUTE_ZERO_PAGE,  // 0x66
   // 0x67
   PLA                       = 0x68,
   ADC_IMMEDIATE ,          // 0x69
   ROR_A,                   // 0x6a
   // 0x6b
   JMP_INDIRECT              = 0x6c,
   ADC_ABSOLUTE,            // 0x6d
   ROR_ABSOLUTE,            // 0x6e
   // 0x6f

   BVS_REL                   = 0x70,
   ADC_Y_INDEXED_INDIRECT,  // 0x71
   // 0x72
   // 0x73
   // 0x74
   ADC_X_ABSOLUTE_ZERO_PAGE  = 0x75,
   ROR_X_ABSOLUTE_ZERO_PAGE,// 0x76
   // 0x77
   SEI                       = 0x78,
   ADC_Y_ABSOLUTE,          // 0x79
   // 0x7a
   // 0x7b
   // 0x7c
   ADC_X_ABSOLUTE            = 0x7d,
   ROR_X_ABSOLUTE,          // 0x7e
   // 0x7f

   // 0x80
   STA_X_INDIRECT_INDEXED    = 0x81,
   // 0x82
   // 0x83
   STY_ABSOLUTE_ZERO_PAGE    = 0x84,
   STA_ABSOLUTE_ZERO_PAGE,  // 0x85
   STX_ABSOLUTE_ZERO_PAGE,  // 0x86
   // 0x87
   DEY                       = 0x88,
   // 0x89
   TXA                       = 0x8a,
   // 0x8b
   STY_ABSOLUTE              = 0x8c,
   STA_ABSOLUTE,            // 0x8d
   STX_ABSOLUTE,            // 0x8e
   // 0x8f

   BCC_REL                   = 0x90,
   STA_Y_INDEXED_INDIRECT,  // 0x91
   // 0x92
   // 0x93
   STY_X_ABSOLUTE_ZERO_PAGE  = 0x94,
   STA_X_ABSOLUTE_ZERO_PAGE,// 0x95,
   STX_Y_ABSOLUTE_ZERO_PAGE,// 0x96
   // 0x97
   TYA                       = 0x98,
   STA_Y_ABSOLUTE,          // 0x99
   TXS,                     // 0x9a
   // 0x9b
   // 0x9c
   STA_X_ABSOLUTE            = 0x9d,
   // 0x9e
   // 0x9f

   LDY_IMMEDIATE             = 0xa0,
   LDA_X_INDIRECT_INDEXED,  // 0xa1
   LDX_IMMEDIATE,           // 0xa2
   // 0xa3
   LDY_ABSOLUTE_ZERO_PAGE    = 0xa4,
   LDA_ABSOLUTE_ZERO_PAGE,  // 0xa5
   LDX_ABSOLUTE_ZERO_PAGE,  // 0xa6
   // 0xa7
   TAY                       = 0xa8,
   LDA_IMMEDIATE,           // 0xa9
   TAX,                     // 0xaa
   // 0xab
   LDY_ABSOLUTE              = 0xac,
   LDA_ABSOLUTE,            // 0xad
   LDX_ABSOLUTE,            // 0xae
   // 0xaf

   BCS_REL                   = 0xb0,
   LDA_Y_INDEXED_INDIRECT,  // 0xb1
   // 0xb2
   // 0xb3
   LDY_X_ABSOLUTE_ZERO_PAGE  = 0xb4,
   LDA_X_ABSOLUTE_ZERO_PAGE,// 0xb5,
   LDX_Y_ABSOLUTE_ZERO_PAGE,// 0xb6
   // 0xb7
   CLV                       = 0xb8,
   LDA_Y_ABSOLUTE,          // 0xb9
   TSX,                     // 0xba
   // 0xbb
   LDY_X_ABSOLUTE            = 0xbc,
   LDA_X_ABSOLUTE,          // 0xbd
   LDX_Y_ABSOLUTE,          // 0xbe
   // 0xbf

   CPY_IMMEDIATE             = 0xc0,
   CMP_X_INDIRECT_INDEXED,  // 0xc1
   // 0xc2
   // 0xc3
   CPY_ABSOLUTE_ZERO_PAGE    = 0xc4,
   CMP_ABSOLUTE_ZERO_PAGE,  // 0xc5
   DEC_ABSOLUTE_ZERO_PAGE,  // 0xc6
   // 0xc7
   INY                       = 0xc8,
   CMP_IMMEDIATE,           // 0xc9
   DEX,                     // 0xca
   // 0xcb
   CPY_ABSOLUTE              = 0xcc,
   CMP_ABSOLUTE,            // 0xcd
   DEC_ABSOLUTE,            // 0xce
   // 0xcf

   BNE_REL                   = 0xd0,
   CMP_Y_INDEXED_INDIRECT,  // 0xd1
   // 0xd2
   // 0xd3
   // 0xd4
   CMP_X_ABSOLUTE_ZERO_PAGE  = 0xd5,
   DEC_X_ABSOLUTE_ZERO_PAGE,// 0xd6
   // 0xd7
   CLD                       = 0xd8,
   CMP_Y_ABSOLUTE,          // 0xd9
   // 0xda
   // 0xdb
   // 0xdc
   CMP_X_ABSOLUTE            = 0xdd,
   DEC_X_ABSOLUTE,          // 0xde
   // 0xdf

   CPX_IMMEDIATE             = 0xe0,
   SBC_X_INDIRECT_INDEXED,  // 0xe1
   // 0xe2
   // 0xe3
   CPX_ABSOLUTE_ZERO_PAGE    = 0xe4,
   SBC_ABSOLUTE_ZERO_PAGE,  // 0xe5
   INC_ABSOLUTE_ZERO_PAGE,  // 0xe6
   // 0xe7
   INX                       = 0xe8,
   SBC_IMMEDIATE,           // 0xe9
   NOP_IMPLIED,             // 0xea
   // 0xeb
   CPX_ABSOLUTE              = 0xec,
   SBC_ABSOLUTE,            // 0xed
   INC_ABSOLUTE,            // 0xee
   // 0xef

   BEQ_REL                   = 0xf0,
   SBC_Y_INDEXED_INDIRECT,  // 0xf1
   // 0xf2
   // 0xf3
   // 0xf4
   SBC_X_ABSOLUTE_ZERO_PAGE  = 0xf5,
   INC_X_ABSOLUTE_ZERO_PAGE,// 0xf6
   // 0xf7
   SED                       = 0xf8,
   SBC_Y_ABSOLUTE,          // 0xf9
   // 0xfa
   // 0xfb
   // 0xfc
   SBC_X_ABSOLUTE            = 0xfd,
   INC_X_ABSOLUTE,          // 0xfe
   // 0xff

};

extern enum OpCodeAddressMode ADDRESS_MODE[];

extern int OP_CODE_LENGTH[];




#endif // CPU6502DEFINES_H
