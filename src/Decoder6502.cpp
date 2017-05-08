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
   case OpCode6502::LDY_IMMEDIATE:
   case OpCode6502::LDA_X_INDIRECT_INDEXED:
   case OpCode6502::LDX_IMMEDIATE:
   case OpCode6502::LDY_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LDA_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LDX_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LDA_IMMEDIATE:
   case OpCode6502::LDY_ABSOLUTE:
   case OpCode6502::LDA_ABSOLUTE:
   case OpCode6502::LDX_ABSOLUTE:
   case OpCode6502::LDA_Y_INDEXED_INDIRECT:
   case OpCode6502::LDY_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LDA_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LDX_Y_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LDA_Y_ABSOLUTE:
   case OpCode6502::LDY_X_ABSOLUTE:
   case OpCode6502::LDA_X_ABSOLUTE:
   case OpCode6502::LDX_Y_ABSOLUTE:
      load(address, opCode);
      break;

   case OpCode6502::STA_X_INDIRECT_INDEXED:
   case OpCode6502::STY_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::STA_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::STX_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::STY_ABSOLUTE:
   case OpCode6502::STA_ABSOLUTE:
   case OpCode6502::STX_ABSOLUTE:
   case OpCode6502::STA_Y_INDEXED_INDIRECT:
   case OpCode6502::STY_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::STA_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::STX_Y_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::STA_Y_ABSOLUTE:
   case OpCode6502::STA_X_ABSOLUTE:
      store(address, opCode);
      break;

   case OpCode6502::TXA:
   case OpCode6502::TYA:
   case OpCode6502::TXS:
   case OpCode6502::TAY:
   case OpCode6502::TAX_IMPLIED:
   case OpCode6502::TSX:
      transfer(address, opCode);
      break;

   case OpCode6502::ADC_X_INDIRECT_INDEXED:
   case OpCode6502::ADC_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ADC_IMMEDIATE:
   case OpCode6502::ADC_ABSOLUTE:
   case OpCode6502::ADC_Y_INDEXED_INDIRECT:
   case OpCode6502::ADC_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ADC_Y_ABSOLUTE:
   case OpCode6502::ADC_X_ABSOLUTE:
      add(address, opCode);
      break;

   case OpCode6502::SBC_X_INDIRECT_INDEXED:
   case OpCode6502::SBC_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::SBC_IMMEDIATE:
   case OpCode6502::SBC_ABSOLUTE:
   case OpCode6502::SBC_Y_INDEXED_INDIRECT:
   case OpCode6502::SBC_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::SBC_Y_ABSOLUTE:
   case OpCode6502::SBC_X_ABSOLUTE:
      subtract(address, opCode);
      break;

   case OpCode6502::INY:
   case OpCode6502::INC_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::INX:
   case OpCode6502::INC_ABSOLUTE:
   case OpCode6502::INC_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::INC_X_ABSOLUTE:
      increment(address, opCode);
      break;

   case OpCode6502::DEY:
   case OpCode6502::DEC_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::DEX_IMPLIED:
   case OpCode6502::DEC_ABSOLUTE:
   case OpCode6502::DEC_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::DEC_X_ABSOLUTE:
      decrement(address, opCode);
      break;

   case OpCode6502::AND_X_INDIRECT_INDEXED:
   case OpCode6502::AND_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::AND_IMMEDIATE:
   case OpCode6502::AND_ABSOLUTE:
   case OpCode6502::AND_Y_INDEXED_INDIRECT:
   case OpCode6502::AND_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::AND_Y_ABSOLUTE:
   case OpCode6502::AND_X_ABSOLUTE:
      andOperation(address, opCode);
      break;

   case OpCode6502::ORA_X_INDIRECT_INDEXED:
   case OpCode6502::ORA_ABSOLUTE_ZEROPAGE:
   case OpCode6502::ORA_ABSOLUTE:
   case OpCode6502::ORA_IMMEDIATE:
   case OpCode6502::ORA_Y_INDEXED_INDIRECT:
   case OpCode6502::ORA_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ORA_Y_ABSOLUTE:
   case OpCode6502::ORA_X_ABSOLUTE:
      orOperation(address, opCode);
      break;

   case OpCode6502::EOR_X_INDIRECT_INDEXED:
   case OpCode6502::EOR_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::EOR_IMMEDIATE:
   case OpCode6502::EOR_ABSOLUTE:
   case OpCode6502::EOR_Y_INDEXED_INDIRECT:
   case OpCode6502::EOR_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::EOR_Y_ABSOLUTE:
   case OpCode6502::EOR_X_ABSOLUTE:
      xorOperation(address, opCode);
      break;

   case OpCode6502::ASL_ABSOLUTE_ZEROPAGE:
   case OpCode6502::ASL_A_IMPLIED:
   case OpCode6502::ASL_ABSOLUTE:
   case OpCode6502::ASL_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ASL_X_ABSOLUTE:
      shiftLeft(address, opCode);
      break;

   case OpCode6502::LSR_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LSR_A:
   case OpCode6502::LSR_ABSOLUTE:
   case OpCode6502::LSR_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::LSR_X_ABSOLUTE:
      shiftRight(address, opCode);
      break;

   case OpCode6502::ROL_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ROL_A_IMPLIED:
   case OpCode6502::ROL_ABSOLUTE:
   case OpCode6502::ROL_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ROL_X_ABSOLUTE:
      rotateLeft(address, opCode);
      break;

   case OpCode6502::ROR_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ROR_A:
   case OpCode6502::ROR_ABSOLUTE:
   case OpCode6502::ROR_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::ROR_X_ABSOLUTE:
      rotateRight(address, opCode);
      break;

   case OpCode6502::PHP:
   case OpCode6502::PHA:
      push(address, opCode);
      break;

   case OpCode6502::PLP:
   case OpCode6502::PLA:
      pull(address, opCode);
      break;

   case OpCode6502::JMP_ABSOLUTE:
   case OpCode6502::JMP_INDIRECT:
      jump(address, opCode);
      break;

   case OpCode6502::JSR_ABSOLUTE:
      jumpSubroutine(address, opCode);
      break;

   case OpCode6502::RTI:
      returnFromInterrupt(address, opCode);
      break;

   case OpCode6502::RTS:
      returnFromSubroutine(address, opCode);
      break;

   case OpCode6502::BIT_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::BIT_ABSOLUTE:
      bitTest(address, opCode);
      break;

   case OpCode6502::CPY_IMMEDIATE:
   case OpCode6502::CMP_X_INDIRECT_INDEXED:
   case OpCode6502::CPY_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::CMP_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::CMP_IMMEDIATE:
   case OpCode6502::CPY_ABSOLUTE:
   case OpCode6502::CMP_ABSOLUTE:
   case OpCode6502::CMP_Y_INDEXED_INDIRECT:
   case OpCode6502::CMP_X_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::CMP_Y_ABSOLUTE:
   case OpCode6502::CMP_X_ABSOLUTE:
   case OpCode6502::CPX_IMMEDIATE:
   case OpCode6502::CPX_ABSOLUTE_ZERO_PAGE:
   case OpCode6502::CPX_ABSOLUTE:
      compare(address, opCode);
      break;

   case OpCode6502::CLC:
   case OpCode6502::CLI:
   case OpCode6502::CLV:
   case OpCode6502::CLD:
      clear(address, opCode);
      break;

   case OpCode6502::SEC:
   case OpCode6502::SEI:
   case OpCode6502::SED:
      set(address, opCode);
      break;

   case OpCode6502::NOP_IMPLIED:
      noOp(address, opCode);
      break;

   case OpCode6502::BRK:
      breakOperation(address, opCode);
      break;

   case OpCode6502::BPL_REL:
   case OpCode6502::BMI_REL:
   case OpCode6502::BVC_REL:
   case OpCode6502::BVS_REL:
   case OpCode6502::BCC_REL:
   case OpCode6502::BCS_REL:
   case OpCode6502::BNE_REL:
   case OpCode6502::BEQ_REL:
      branch(address, opCode);
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
      return getAbsoluteOperand(addr);

   case OpCodeAddressMode::ABSOLUTE_ZERO_PAGE:
      return getAbsoluteZeroPageOperand(addr);

   case OpCodeAddressMode::IMPLIED:
      LOG_WARNING() << "getOperand called on implied op code: " << Utils::toHex8(opCode);
      return 0;

   case OpCodeAddressMode::INDEXED:
      return getIndexedOperand(addr);

   case OpCodeAddressMode::INDEXED_ZERO_PAGE:
      return getIndexedZeroPageOperand(addr);

   case OpCodeAddressMode::INDIRECT:
      LOG_WARNING() << "getOperand called on indirect op code (operand is 16-bit): " << Utils::toHex8(opCode);
      return 0;

   case OpCodeAddressMode::RELATIVE:
      LOG_WARNING() << "getOperand called on indirect op code (operand is 16-bit): " << Utils::toHex8(opCode);
      return 0;

   case OpCodeAddressMode::INDIRECT_INDEXED:
      return getIndirectIndexedOperand(addr);

   case OpCodeAddressMode::INDEXED_INDIRECT:
      return getIndexedIndirectOperand(addr);

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

uint8_t Decoder6502::getAbsoluteOperand(CpuAddress addr)
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

uint8_t Decoder6502::getAbsoluteZeroPageOperand(CpuAddress addr)
{
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (failed during operand fetch), no memory device for address" << addressToString(addr + 1);
      return 0;
   }

   CpuAddress memoryLocation = 0x0000;
   memoryLocation = mem->read8(addr + 1);

   mem = theMemoryController->getDevice(memoryLocation);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (invalid address provided by operand), no memory device for address" << addressToString(memoryLocation);
      return 0;
   }

   return mem->read8(memoryLocation);
}

uint8_t Decoder6502::getIndexedOperand(CpuAddress addr)
{
   LOG_FATAL() << "Not implemented";
   return 0;
}

uint8_t Decoder6502::getIndexedZeroPageOperand(CpuAddress addr)
{
   LOG_FATAL() << "Not implemented";
   return 0;
}

uint8_t Decoder6502::getIndirectIndexedOperand(CpuAddress addr)
{
   LOG_FATAL() << "Not implemented";
   return 0;
}

uint8_t Decoder6502::getIndexedIndirectOperand(CpuAddress addr)
{
   LOG_FATAL() << "Not implemented";
   return 0;
}

uint16_t Decoder6502::getRelativeOperand(CpuAddress addr)
{
   LOG_FATAL() << "Not implemented";
   return 0;
}

uint16_t Decoder6502::getIndirectOperand(CpuAddress addr)
{
   LOG_FATAL() << "Not implemented";
   return 0;
}
