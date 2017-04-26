#include "Cpu6502Defines.h"

enum OpCodeAddressMode ADDRESS_MODE[] =
{
   OpCodeAddressMode::IMPLIED,            // 0x00
   OpCodeAddressMode::INDIRECT_INDEXED,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::IMPLIED,
   OpCodeAddressMode::IMMEDIATE,
   OpCodeAddressMode::IMPLIED,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,

   OpCodeAddressMode::RELATIVE
};





int OP_CODE_LENGTH[] =
{
   2, // IMMEDIATE
   3, // ABSOLUTE
   2, // ABSOLUTE_ZERO_PAGE
   1, // IMPLIED
   3, // INDEXED
   2, // INDEXED_ZERO_PAGE
   3, // INDIRECT
   2, // RELATIVE
   2, // INDIRECT_INDEXED
   2  // INDEXED_INDIRECT
};


