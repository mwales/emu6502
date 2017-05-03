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

   OpCodeAddressMode::RELATIVE,           // 0x10
   OpCodeAddressMode::INDEXED_INDIRECT,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::IMPLIED,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,

   OpCodeAddressMode::ABSOLUTE,           // 0x20
   OpCodeAddressMode::INDIRECT_INDEXED,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::IMPLIED,
   OpCodeAddressMode::IMMEDIATE,
   OpCodeAddressMode::IMPLIED,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,

   OpCodeAddressMode::RELATIVE,           // 0x30
   OpCodeAddressMode::INDEXED_INDIRECT,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::IMPLIED,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,

   OpCodeAddressMode::IMPLIED,           // 0x40
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
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,

   OpCodeAddressMode::RELATIVE,           // 0x50
   OpCodeAddressMode::INDEXED_INDIRECT,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::ABSOLUTE_ZERO_PAGE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::IMPLIED,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::INVALID,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::ABSOLUTE,
   OpCodeAddressMode::INVALID,

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


