#ifndef DISASSEMBLER6502_H
#define DISASSEMBLER6502_H

#include "Decoder6502.h"
#include <iostream>
#include <map>


/**
 * Class outputs the disassembly in the debugger or if dumping assembly using the standalone
 * disassembler application.
 */
class Disassembler6502 : public Decoder6502
{
public:
   Disassembler6502(MemoryController* memCtrl);

   virtual ~Disassembler6502();

   virtual void start(CpuAddress address);

   virtual void addExtraEntryPoint(CpuAddress address);

   virtual void halt();

   void printDisassembly();

   void includeOpCodes(bool val);

   void includeAddress(bool val);

   void addDisassemblerListing(OpCodeInfo* oci);

   std::string disassembleInstruction(OpCodeInfo* oci);

   /**
    * Returns a short code listing (intended use by the debugger)
    * @param addr Address to start disassembly listing at
    * @param numInstructions Num instructions to disassemble
    * @return Code listing with address and opcodes
    */
   std::string debugListing(CpuAddress addr, int numInstructions);


protected:

   // All the virtual void methods inherited from parent decoder
   // All of the op code handler functions (auto-generated code below)
   virtual void handler_and(OpCodeInfo* oci);
   virtual void handler_bvs(OpCodeInfo* oci);
   virtual void handler_sec(OpCodeInfo* oci);
   virtual void handler_rol(OpCodeInfo* oci);
   virtual void handler_pla(OpCodeInfo* oci);
   virtual void handler_anc(OpCodeInfo* oci);
   virtual void handler_rti(OpCodeInfo* oci);
   virtual void handler_arr(OpCodeInfo* oci);
   virtual void handler_rra(OpCodeInfo* oci);
   virtual void handler_bvc(OpCodeInfo* oci);
   virtual void handler_sax(OpCodeInfo* oci);
   virtual void handler_lsr(OpCodeInfo* oci);
   virtual void handler_rts(OpCodeInfo* oci);
   virtual void handler_inx(OpCodeInfo* oci);
   virtual void handler_ror(OpCodeInfo* oci);
   virtual void handler_ldx(OpCodeInfo* oci);
   virtual void handler_alr(OpCodeInfo* oci);
   virtual void handler_ahx(OpCodeInfo* oci);
   virtual void handler_sei(OpCodeInfo* oci);
   virtual void handler_iny(OpCodeInfo* oci);
   virtual void handler_inc(OpCodeInfo* oci);
   virtual void handler_cli(OpCodeInfo* oci);
   virtual void handler_beq(OpCodeInfo* oci);
   virtual void handler_cpy(OpCodeInfo* oci);
   virtual void handler_cld(OpCodeInfo* oci);
   virtual void handler_txs(OpCodeInfo* oci);
   virtual void handler_tas(OpCodeInfo* oci);
   virtual void handler_clc(OpCodeInfo* oci);
   virtual void handler_adc(OpCodeInfo* oci);
   virtual void handler_tsx(OpCodeInfo* oci);
   virtual void handler_xaa(OpCodeInfo* oci);
   virtual void handler_clv(OpCodeInfo* oci);
   virtual void handler_asl(OpCodeInfo* oci);
   virtual void handler_jmp(OpCodeInfo* oci);
   virtual void handler_bne(OpCodeInfo* oci);
   virtual void handler_ldy(OpCodeInfo* oci);
   virtual void handler_axs(OpCodeInfo* oci);
   virtual void handler_plp(OpCodeInfo* oci);
   virtual void handler_tax(OpCodeInfo* oci);
   virtual void handler_pha(OpCodeInfo* oci);
   virtual void handler_bmi(OpCodeInfo* oci);
   virtual void handler_rla(OpCodeInfo* oci);
   virtual void handler_tya(OpCodeInfo* oci);
   virtual void handler_tay(OpCodeInfo* oci);
   virtual void handler_sbc(OpCodeInfo* oci);
   virtual void handler_lax(OpCodeInfo* oci);
   virtual void handler_txa(OpCodeInfo* oci);
   virtual void handler_jsr(OpCodeInfo* oci);
   virtual void handler_kil(OpCodeInfo* oci);
   virtual void handler_bit(OpCodeInfo* oci);
   virtual void handler_php(OpCodeInfo* oci);
   virtual void handler_nop(OpCodeInfo* oci);
   virtual void handler_dcp(OpCodeInfo* oci);
   virtual void handler_ora(OpCodeInfo* oci);
   virtual void handler_dex(OpCodeInfo* oci);
   virtual void handler_dey(OpCodeInfo* oci);
   virtual void handler_dec(OpCodeInfo* oci);
   virtual void handler_sed(OpCodeInfo* oci);
   virtual void handler_sta(OpCodeInfo* oci);
   virtual void handler_sre(OpCodeInfo* oci);
   virtual void handler_shx(OpCodeInfo* oci);
   virtual void handler_shy(OpCodeInfo* oci);
   virtual void handler_bpl(OpCodeInfo* oci);
   virtual void handler_bcc(OpCodeInfo* oci);
   virtual void handler_cpx(OpCodeInfo* oci);
   virtual void handler_eor(OpCodeInfo* oci);
   virtual void handler_lda(OpCodeInfo* oci);
   virtual void handler_slo(OpCodeInfo* oci);
   virtual void handler_las(OpCodeInfo* oci);
   virtual void handler_isc(OpCodeInfo* oci);
   virtual void handler_brk(OpCodeInfo* oci);
   virtual void handler_cmp(OpCodeInfo* oci);
   virtual void handler_stx(OpCodeInfo* oci);
   virtual void handler_sty(OpCodeInfo* oci);
   virtual void handler_bcs(OpCodeInfo* oci);
   // End of auto generated code

   // Overrides of base methods
   virtual void preHandlerHook(OpCodeInfo* oci);
   virtual void postHandlerHook(OpCodeInfo* oci);

   virtual void updatePc(uint8_t bytesIncrement);

   void printOpCodes(std::string* listingText, CpuAddress addr, int opCode);

   std::string getOpCodeString(CpuAddress addr);

   void printAddress(std::string* listingText, CpuAddress addr);

   std::string addJumpLabelStatement(CpuAddress instAddr, char const * const labelPrefix);

   std::string addBranchLabelFromRelativeOffset(uint8_t offset);

   bool theDeadEndFlag;
   CpuAddress thePc;

   std::vector<CpuAddress> theEntryPoints;

   std::map<CpuAddress, std::string> theListing;

   std::map<CpuAddress, std::string> theLabels;

   bool theHaltFlag;

   bool thePrintOpCodeFlag;

   bool thePrintAddressFlag;

};


#endif // DISASSEMBLER6502_H
