#include "Decoder6502.h"
#include "Logger.h"
#include "Utils.h"
#include "MemoryDev.h"
#include "MemoryController.h"




void Decoder6502::decode(CpuAddress address)
{
   MemoryDev* mem = theMemoryController->getDevice(address);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed, no memory device for address" << addressToString(address);
      return;
   }

   uint8_t opCode = mem->read8(address);

   switch( (enum OpCode6502) opCode)
   {
   case OpCode6502::BRK:
      breakOperation(address, opCode);
      return;

   case OpCode6502::ORA_ABSOLUTE:
   case OpCode6502::ORA_ABSOLUTE_ZEROPAGE:
   case OpCode6502::ORA_IMMEDIATE:
   case OpCode6502::ORA_X_INDIRECT_INDEXED:
      orOperation(address, opCode);
      return;

   default:

      LOG_WARNING() << "Unknown opcode" << Utils::toHex8(opCode)
                    << "at address" << addressToString(address);
   }
}
