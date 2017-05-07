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
      break;

   case OpCode6502::ORA_ABSOLUTE:

   case OpCode6502::ORA_ABSOLUTE_ZEROPAGE:
   case OpCode6502::ORA_IMMEDIATE:
   case OpCode6502::ORA_X_INDIRECT_INDEXED:
      orOperation(address, opCode);
      break;

   case OpCode6502::ASL_ABSOLUTE_ZEROPAGE:
   case OpCode6502::ASL_A_IMPLIED:
   case OpCode6502::ASL_ABSOLUTE:
      shiftLeft(address, opCode);
      break;

   case OpCode6502::PUSH_PROC_STATUS:
      push(address, opCode);
      break;

   default:

      LOG_WARNING() << "Unknown opcode" << Utils::toHex8(opCode)
                    << "at address" << addressToString(address);
   }

   updatePc(OP_CODE_LENGTH[(int) ADDRESS_MODE[opCode]]);
}

uint8_t Decoder6502::getOperand(CpuAddress addr, uint8_t opCode)
{
   OpCodeAddressMode mode = ADDRESS_MODE[opCode];

   switch(mode)
   {
   case OpCodeAddressMode::IMMEDIATE:
      return getImmediateOperand(addr);

   case OpCodeAddressMode::ABSOLUTE:
      return getAbsolute(addr);

   case OpCodeAddressMode::ABSOLUTE_ZERO_PAGE:

   case OpCodeAddressMode::IMPLIED:

   case OpCodeAddressMode::INDEXED:

   case OpCodeAddressMode::INDEXED_ZERO_PAGE:

   case OpCodeAddressMode::INDIRECT:

   case OpCodeAddressMode::RELATIVE:

   case OpCodeAddressMode::INDIRECT_INDEXED:

   case OpCodeAddressMode::INDEXED_INDIRECT:
      LOG_WARNING() << __PRETTY_FUNCTION__ << "not implemented yet";
      break;

   case OpCodeAddressMode::INVALID:
      LOG_WARNING() << "OpCode" << Utils::toHex8(opCode) << "is not valid op code for 6502 (@" << addressToString(addr) << ")";
      return 0;
      break;

   default:
      LOG_FATAL() << "Internal error when getting operand for op code" << Utils::toHex8(opCode);

   }

   return 0;
}

uint8_t Decoder6502::getImmediateOperand(CpuAddress addr)
{
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed, no memory device for address" << addressToString(addr + 1);
      return 0;
   }

   return mem->read8(addr+1);
}

uint8_t Decoder6502::getAbsolute(CpuAddress addr)
{
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (failed during operand fetch), no memory device for address" << addressToString(addr + 1);
      return 0;
   }

   CpuAddress memoryLocation = mem->read16(addr + 1);

   mem = theMemoryController->getDevice(memoryLocation);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (invalid address provided by operand), no memory device for address" << addressToString(memoryLocation);
      return 0;
   }

   return mem->read8(memoryLocation);
}
