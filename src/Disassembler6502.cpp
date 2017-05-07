#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include "Disassembler6502.h"
#include "Logger.h"
#include "MemoryDev.h"
#include "MemoryController.h"





Disassembler6502::Disassembler6502(MemoryController* memCtrl):
   thePrintOpCodeFlag(false),
   thePrintAddressFlag(false)
{
   theMemoryController = memCtrl;
}

void Disassembler6502::start(CpuAddress address)
{
   theEntryPoints.push_back(address);

   while(!theEntryPoints.empty())
   {
      CpuAddress nextAddress = theEntryPoints.back();
      theEntryPoints.pop_back();
      LOG_DEBUG() << "Next Entry Point =" << addressToString(nextAddress);

      if (theListing.find(nextAddress) == theListing.end())
      {
         std::ostringstream oss;
         oss << "entry_" << addressToString(nextAddress) << ":";

         theLabels[nextAddress] = oss.str();
         theDeadEndFlag = false;
         thePc = nextAddress;

         while(!theDeadEndFlag)
         {


            decode(thePc);



            // Delete me!!!
            //theDeadEndFlag = true;
         }
      }
      else
      {
         LOG_DEBUG() << "Entry point @" << addressToString(nextAddress) << "already analyzed";
      }
   }
}

void Disassembler6502::halt()
{
   theHaltFlag = true;
   theDeadEndFlag = true;
}

void Disassembler6502::printDisassembly()
{
   for(auto lineIter = theListing.begin(); lineIter != theListing.end(); lineIter++)
   {
      std::cout << lineIter->second << std::endl;
   }
}


void Disassembler6502::includeOpCodes(bool val)
{
   thePrintOpCodeFlag = val;
}

void Disassembler6502::includeAddress(bool val)
{
   thePrintAddressFlag = val;
}

void Disassembler6502::printOpCodes(std::string* listingText, CpuAddress addr, int opCode)
{
   // opcode fmt:                     "xx xx xx  "
   char const * const EMPTY_OP_CODE = "          ";
   const int OP_CODE_TEXT_LEN = 11;
   char opCodeText[OP_CODE_TEXT_LEN];

   int numOpCodes = 1;
   if (!thePrintOpCodeFlag)
   {
      numOpCodes = 0;
   }
   else
   {
      numOpCodes = OP_CODE_LENGTH[ (int) ADDRESS_MODE[opCode]];
   }

   LOG_DEBUG() << "Printing" << numOpCodes << "opcode(s) for instruction at address" << addressToString(addr);

   switch(numOpCodes)
   {
   case 1:
      snprintf(opCodeText, OP_CODE_TEXT_LEN, "%02x        ", theMemoryController->getDevice(addr)->read8(addr));
      break;

   case 2:
      snprintf(opCodeText, OP_CODE_TEXT_LEN, "%02x %02x     ", theMemoryController->getDevice(addr)->read8(addr),
               theMemoryController->getDevice(addr+1)->read8(addr+1));
      break;

   case 3:
      snprintf(opCodeText, OP_CODE_TEXT_LEN, "%02x %02x %02x  ", theMemoryController->getDevice(addr)->read8(addr),
               theMemoryController->getDevice(addr+1)->read8(addr+1),
               theMemoryController->getDevice(addr + 2)->read8(addr + 2));
      break;

   case 0:
   default:
      strncpy(opCodeText, EMPTY_OP_CODE, OP_CODE_TEXT_LEN);
   }

   listingText->append(opCodeText);

}

void Disassembler6502::printAddress(std::string* listingText, CpuAddress addr)
{
   if (thePrintAddressFlag)
   {
      listingText->append(addressToString(addr));
      *listingText += "   ";
   }
}

void Disassembler6502::load(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::store(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::transfer(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::add(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::subtract(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::increment(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::decrement(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::andOperation(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::orOperation(CpuAddress instAddr, uint8_t opCodes)
{
   std::string listingData;
   printAddress(&listingData, instAddr);
   printOpCodes(&listingData, instAddr, opCodes);
   listingData += "ORA ";

   listingData += getOperandText(instAddr, opCodes);

   theListing[instAddr] = listingData;
}

void Disassembler6502::xorOperation(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::shiftLeft(CpuAddress instAddr, uint8_t opCodes)
{
   std::string listingData;
   printAddress(&listingData, instAddr);
   printOpCodes(&listingData, instAddr, opCodes);
   listingData += "ASL ";

   listingData += getOperandText(instAddr, opCodes);

   theListing[instAddr] = listingData;
}

void Disassembler6502::shiftRight(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::rotateLeft(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::rotateRight(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::push(CpuAddress instAddr, uint8_t opCodes)
{
   std::string listingData;
   printAddress(&listingData, instAddr);
   printOpCodes(&listingData, instAddr, opCodes);

   if (opCodes == (uint8_t) OpCode6502::PUSH_PROC_STATUS)
   {
      listingData += "PHP";
   }
   else
   {
      listingData += "PHA";
   }

   theListing[instAddr] = listingData;
}

void Disassembler6502::pull(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::jump(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::jumpSubroutine(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::returnFromInterrupt(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::returnFromSubroutine(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::bitTest(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::compare(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::clear(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::set(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::noOp(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

void Disassembler6502::breakOperation(CpuAddress instAddr, uint8_t opCodes)
{
   std::string listingData;
   printAddress(&listingData, instAddr);
   printOpCodes(&listingData, instAddr, opCodes);
   listingData += "BRK";

   theListing[instAddr] = listingData;
}

void Disassembler6502::branch(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}

std::string Disassembler6502::getOperandText(CpuAddress addr, uint8_t opCode)
{
   OpCodeAddressMode mode = ADDRESS_MODE[opCode];

   switch(mode)
   {
   case OpCodeAddressMode::IMMEDIATE:
      return getImmediateOpText(addr);

   case OpCodeAddressMode::ABSOLUTE:
      return getAbsoluteOpText(addr);

   case OpCodeAddressMode::ABSOLUTE_ZERO_PAGE:
      return getAbsoluteZeroOpText(addr);

   case OpCodeAddressMode::IMPLIED:
      // These instructions have no operand
      return "";

   case OpCodeAddressMode::INDEXED:
      return getIndexedOpText(addr);

   case OpCodeAddressMode::INDEXED_ZERO_PAGE:
      return getIndexedZeroPageOpText(addr);

   case OpCodeAddressMode::INDIRECT:
      return getIndirectOpText(addr);

   case OpCodeAddressMode::RELATIVE:
      return getRelativeOpText(addr);

   case OpCodeAddressMode::INDIRECT_INDEXED:
      return getIndirectIndexedOpText(addr);

   case OpCodeAddressMode::INDEXED_INDIRECT:
      return getIndexedIndirectOpText(addr);

   case OpCodeAddressMode::INVALID:
      LOG_WARNING() << "OpCode" << Utils::toHex8(opCode) << "is not valid op code for 6502 (@" << addressToString(addr) << ")";
      return "<ERROR>";
      break;

   default:
      LOG_FATAL() << "Internal error when getting operand for op code" << Utils::toHex8(opCode);

   }

   return "<ERROR2>";
}

std::string Disassembler6502::getImmediateOpText(CpuAddress addr)
{
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed, no memory device for address" << addressToString(addr + 1);
      return 0;
   }

   char buf[10];
   snprintf(buf, 10, "#$%02x", mem->read8(addr + 1));

   return buf;
}

std::string Disassembler6502::getAbsoluteOpText(CpuAddress addr)
{
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (failed during operand fetch), no memory device for address" << addressToString(addr + 1);
      return "<FAIL>";
   }

   char buf[10];
   snprintf(buf, 10, "$%04x", mem->read16(addr + 1));

   return buf;
}

std::string Disassembler6502::getAbsoluteZeroOpText(CpuAddress addr)
{
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (failed during operand fetch), no memory device for address" << addressToString(addr + 1);
      return "<FAIL>";
   }

   char buf[10];
   snprintf(buf, 10, "$%02x", mem->read8(addr + 1));

   return buf;
}

std::string Disassembler6502::getIndexedOpText(CpuAddress addr)
{
   // Some of these instructions use the X register for indexing, and some Y
   MemoryDev* mem = theMemoryController->getDevice(addr);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (failed during operand fetch), no memory device for address" << addressToString(addr + 1);
      return "<FAIL>";
   }

   uint8_t opCode = mem->read8(addr);

   uint16_t addressToIndex = mem->read16(addr+1);

   uint8_t lowNibble = opCode & 0x0f;

   char buf[10];
   // Operations that index with why have a lower nibble of 0x09, with the exception of LDX abs, Y (BE)
   if ( (lowNibble == 0x09) || (opCode == 0xbe) )
   {
      snprintf(buf, 10, "$%04x, Y", addressToIndex);
   }
   else
   {
      snprintf(buf, 10, "$%04x, X", addressToIndex);
   }

   return buf;
}

std::string Disassembler6502::getIndexedZeroPageOpText(CpuAddress addr)
{
   return "not implemented";
}

std::string Disassembler6502::getIndirectOpText(CpuAddress addr)
{
return "not implemented";
}

std::string Disassembler6502::getRelativeOpText(CpuAddress addr)
{
return "not implemented";
}

std::string Disassembler6502::getIndirectIndexedOpText(CpuAddress addr)
{
   // These are always with the X register
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (failed during operand fetch), no memory device for address" << addressToString(addr + 1);
      return "<FAIL>";
   }

   char buf[20];
   snprintf(buf, 20, "($%02x, X)", mem->read8(addr + 1));

   return buf;
}

std::string Disassembler6502::getIndexedIndirectOpText(CpuAddress addr)
{
   // These are always with the Y register
   MemoryDev* mem = theMemoryController->getDevice(addr + 1);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed (failed during operand fetch), no memory device for address" << addressToString(addr + 1);
      return "<FAIL>";
   }

   char buf[20];
   snprintf(buf, 20, "($%02x), Y", mem->read8(addr + 1));

   return buf;
}


void Disassembler6502::updatePc(uint8_t bytesIncrement)
{
   LOG_DEBUG() << "PC Incrementing" << (int) bytesIncrement;
   thePc += bytesIncrement;
}
