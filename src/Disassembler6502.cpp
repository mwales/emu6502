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

   constructCpuGlobals();
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
         oss << "entry_" << addressToString(nextAddress);

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

void Disassembler6502::addExtraEntryPoint(CpuAddress address)
{
   theEntryPoints.push_back(address);
}

void Disassembler6502::halt()
{
   theHaltFlag = true;
   theDeadEndFlag = true;
}

void Disassembler6502::printDisassembly()
{
   std::vector<MemoryRange> ranges = theMemoryController->getOrderedRangeList();
   for(auto curRange = ranges.begin(); curRange != ranges.end(); curRange++)
   {
      // Dissassemble all address from each range of memory available
      CpuAddress curDisAddr = curRange->first;
      while(curDisAddr <= curRange->second)
      {
         // Is there a label for this address?
         if (theLabels.find(curDisAddr) != theLabels.end())
         {
            std::cout << theLabels[curDisAddr] << ":" << std::endl;
         }

         uint8_t curAddrVal = theMemoryController->getDevice(curDisAddr)->read8(curDisAddr);

         // Is there a listing for this address?
         if (theListing.find(curDisAddr) != theListing.end())
         {
            // Print out the address if configured
            if(thePrintAddressFlag)
            {
               std::cout << Utils::toHex16(curDisAddr, false) << "   ";
            }

            std::cout << theListing[curDisAddr] << std::endl;
            //curDisAddr += OP_CODE_LENGTH[(int) ADDRESS_MODE[curAddrVal]];
            curDisAddr += gOpCodes[curAddrVal].theNumBytes;
         }
         else
         {
            // This is just a raw databyte
            std::cout << addressToString(curDisAddr) << " DB $" << Utils::toHex8(curAddrVal) << std::endl;

            curDisAddr++;
         }

      }
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
      numOpCodes = gOpCodes[opCode].theNumBytes;
   }

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
               theMemoryController->getDevice(addr + 1)->read8(addr + 1),
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

std::string Disassembler6502::addJumpLabelStatement(CpuAddress destAddr, char const * const prefix)
{
   char buf[20];

   snprintf(buf, 20, "%s_0x%04x", prefix, destAddr);

   theLabels[destAddr] = buf;

   LOG_DEBUG() << "Adding an entry point for disassembly @ " << addressToString(destAddr);
   theEntryPoints.push_back(destAddr);

   return buf;
}

std::string Disassembler6502::addBranchLabelFromRelativeOffset(uint8_t offset)
{
   int8_t signedOffset = (int8_t) offset;
   int signedAddr = signedOffset + thePc;
   CpuAddress targetAddr = signedAddr;

   addJumpLabelStatement(targetAddr, "branch");

   std::string retVal;

   retVal = "branch_";
   retVal += Utils::toHex16(targetAddr);
   return retVal;
}

void Disassembler6502::updatePc(uint8_t bytesIncrement)
{
   LOG_DEBUG() << "PC Incrementing" << (int) bytesIncrement;
   thePc += bytesIncrement;
}

void Disassembler6502::addDisassemblerListing(OpCodeInfo* oci)
{
   std::string listingText;


   if (thePrintOpCodeFlag)
   {
      printOpCodes(&listingText, thePc, oci->theOpCode);
   }

   listingText += oci->theMnemonicDisass;
   listingText += " ";

   uint8_t op2, op3;


   // Need to print out some disassembly based on the addressing mode now
   switch(gOpCodes[oci->theOpCode].theAddrMode)
   {
   case IMPLIED:
      // Do nothing
      break;

   case IMMEDIATE:
      listingText += "#$";
      listingText += Utils::toHex8(theOpCode2, false);
      break;

   case RELATIVE:

      listingText += "*";

      if ((theOpCode2 & 0x80)== 0x80)
      {
         // Negative jump

         listingText += "-";
         // 2's complement
         uint8_t op2 = theOpCode2 - 1;
         op2 = ~op2;
         listingText += Utils::toHex8(op2, false);
      }
      else
      {
         // Positive jump
         listingText += "+";
         listingText += Utils::toHex8(theOpCode2, false);
      }

      break;

   case INDIRECT:
      listingText += "($";
      listingText += Utils::toHex8(theOpCode3, false);
      listingText += Utils::toHex8(theOpCode2, false);
      listingText += ")";
      break;

   case INDIRECT_X:
      listingText += "($";
      listingText += Utils::toHex8(theOpCode2, false);
      listingText += ",X)";
      break;

   case INDIRECT_Y:
      listingText += "($";
      listingText += Utils::toHex8(theOpCode2, false);
      listingText += "),Y";
      break;

   case ABSOLUTE:
      listingText += "$";
      listingText += Utils::toHex8(theOpCode3, false);
      listingText += Utils::toHex8(theOpCode2, false);
      break;

   case ZERO_PAGE:
      listingText += "$";
      listingText += Utils::toHex8(theOpCode2, false);
      break;

   case ABS_ZP_X:
      listingText += "$";
      listingText += Utils::toHex8(theOpCode3, false);
      listingText += Utils::toHex8(theOpCode2, false);
      listingText += ",X";
      break;

   case ABS_ZP_Y:
      listingText += "$";
      listingText += Utils::toHex8(theOpCode3, false);
      listingText += Utils::toHex8(theOpCode2, false);
      listingText += ",Y";
      break;

   case ZERO_PAGE_X:
      listingText += "$";
      listingText += Utils::toHex8(theOpCode2, false);
      listingText += ",X";
      break;

   case ZERO_PAGE_Y:
      listingText += "$";
      listingText += Utils::toHex8(theOpCode2, false);
      listingText += ",Y";
      break;

   }

   theListing[thePc] = listingText;
}

void Disassembler6502::preHandlerHook(OpCodeInfo* oci)
{
   // This is where sub-classes will do sub-class specific behavior if necessary
   addDisassemblerListing(oci);
}

void Disassembler6502::postHandlerHook(OpCodeInfo* oci)
{
   // This is where sub-classes will do sub-class specific behavior if necessary
}


void Disassembler6502::handler_and(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for and";
   addDisassemblerListing(oci);
}

void Disassembler6502::handler_bvs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-empty handler for bvs";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}

void Disassembler6502::handler_sec(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sec";
}

void Disassembler6502::handler_rol(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rol";
}

void Disassembler6502::handler_pla(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for pla";
}

void Disassembler6502::handler_anc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for anc";
}

void Disassembler6502::handler_rti(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rti";
}

void Disassembler6502::handler_arr(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for arr";
}

void Disassembler6502::handler_rra(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rra";
}

void Disassembler6502::handler_bvc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-empty handler for bvc";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}

void Disassembler6502::handler_sax(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sax";
}

void Disassembler6502::handler_lsr(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for lsr";
}

void Disassembler6502::handler_rts(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rts";
}

void Disassembler6502::handler_inx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for inx";
}

void Disassembler6502::handler_ror(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ror";
}

void Disassembler6502::handler_ldx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ldx";
}

void Disassembler6502::handler_alr(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for alr";
}

void Disassembler6502::handler_ahx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ahx";
}

void Disassembler6502::handler_sei(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sei";
}

void Disassembler6502::handler_iny(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for iny";
}

void Disassembler6502::handler_inc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for inc";
}

void Disassembler6502::handler_cli(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cli";
}

void Disassembler6502::handler_beq(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-empty handler for beq";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}

void Disassembler6502::handler_cpy(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cpy";
}

void Disassembler6502::handler_cld(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cld";
}

void Disassembler6502::handler_txs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for txs";
}

void Disassembler6502::handler_tas(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tas";
}

void Disassembler6502::handler_clc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for clc";
}

void Disassembler6502::handler_adc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for adc";
}

void Disassembler6502::handler_tsx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tsx";
}

void Disassembler6502::handler_xaa(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for xaa";
}

void Disassembler6502::handler_clv(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for clv";
}

void Disassembler6502::handler_asl(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for asl";
}

void Disassembler6502::handler_jmp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for jmp";
}

void Disassembler6502::handler_bne(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-empty handler for bne";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}

void Disassembler6502::handler_ldy(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ldy";
}

void Disassembler6502::handler_axs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for axs";
}

void Disassembler6502::handler_plp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for plp";
}

void Disassembler6502::handler_tax(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tax";
}

void Disassembler6502::handler_pha(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for pha";
}

void Disassembler6502::handler_bmi(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-empty handler for bmi";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}

void Disassembler6502::handler_rla(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rla";
}

void Disassembler6502::handler_tya(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tya";
}

void Disassembler6502::handler_tay(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tay";
}

void Disassembler6502::handler_sbc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sbc";
}

void Disassembler6502::handler_lax(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for lax";
}

void Disassembler6502::handler_txa(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for txa";
}

void Disassembler6502::handler_jsr(OpCodeInfo* oci)
{
   // Is this the absolute address insturction?
   if (oci->theAddrMode == ABSOLUTE)
   {
      uint16_t targetAddr;

      targetAddr = ((uint16_t) theOpCode3 << 8) + (uint16_t) theOpCode2 + oci->theNumBytes;

      std::string label = addJumpLabelStatement(targetAddr, "sub");

      std::string newListing = theListing[thePc];
      newListing += " ; ";
      newListing += label;
      theListing[thePc] = newListing;

      LOG_DEBUG() << "Non-Empty handler for jsr to target @" << Utils::toHex16(targetAddr);
   }
   else
   {
      LOG_DEBUG() << "Empty handler for jsr";
   }
}

void Disassembler6502::handler_kil(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for kil";
}

void Disassembler6502::handler_bit(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bit";
}

void Disassembler6502::handler_php(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for php";
}

void Disassembler6502::handler_nop(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for nop";
}

void Disassembler6502::handler_dcp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dcp";
}

void Disassembler6502::handler_ora(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ora";
}

void Disassembler6502::handler_dex(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dex";
}

void Disassembler6502::handler_dey(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dey";
}

void Disassembler6502::handler_dec(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dec";
}

void Disassembler6502::handler_sed(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sed";
}

void Disassembler6502::handler_sta(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sta";
}

void Disassembler6502::handler_sre(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sre";
}

void Disassembler6502::handler_shx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for shx";
}

void Disassembler6502::handler_shy(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for shy";
}

void Disassembler6502::handler_bpl(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-Empty handler for bpl";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}

void Disassembler6502::handler_bcc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-empty handler for bcc";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}

void Disassembler6502::handler_cpx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cpx";
}

void Disassembler6502::handler_eor(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for eor";
}

void Disassembler6502::handler_lda(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for lda";
}

void Disassembler6502::handler_slo(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for slo";
}

void Disassembler6502::handler_las(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for las";
}

void Disassembler6502::handler_isc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for isc";
}

void Disassembler6502::handler_brk(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for brk";
}

void Disassembler6502::handler_cmp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cmp";
}

void Disassembler6502::handler_stx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for stx";
}

void Disassembler6502::handler_sty(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sty";
}

void Disassembler6502::handler_bcs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Non-empty handler for bcs";

   std::string branchName = addBranchLabelFromRelativeOffset(theOpCode2 + oci->theNumBytes);
   theListing[thePc] += " ; ";
   theListing[thePc] += branchName;
}
