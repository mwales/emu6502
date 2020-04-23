#ifndef DECODER6502_H
#define DECODER6502_H

#include "Decoder.h"

#include <stdint.h>

/**
 * MOS 6502 instruction decoding.  This class is abstract because it will not implement the halt()
 * operation
 */
class Decoder6502 : public Decoder
{
public:

   // I want these handlers to be protected, but then i guess you can't function pointers to them
   // then, maybe I will bring in the Cpu6501Defines.h code into this class...

   // All of the op code handler functions (auto-generated code below)
   virtual void handler_and(OpCodeInfo* oci) = 0;
   virtual void handler_bvs(OpCodeInfo* oci) = 0;
   virtual void handler_sec(OpCodeInfo* oci) = 0;
   virtual void handler_rol(OpCodeInfo* oci) = 0;
   virtual void handler_pla(OpCodeInfo* oci) = 0;
   virtual void handler_anc(OpCodeInfo* oci) = 0;
   virtual void handler_rti(OpCodeInfo* oci) = 0;
   virtual void handler_arr(OpCodeInfo* oci) = 0;
   virtual void handler_rra(OpCodeInfo* oci) = 0;
   virtual void handler_bvc(OpCodeInfo* oci) = 0;
   virtual void handler_sax(OpCodeInfo* oci) = 0;
   virtual void handler_lsr(OpCodeInfo* oci) = 0;
   virtual void handler_rts(OpCodeInfo* oci) = 0;
   virtual void handler_inx(OpCodeInfo* oci) = 0;
   virtual void handler_ror(OpCodeInfo* oci) = 0;
   virtual void handler_ldx(OpCodeInfo* oci) = 0;
   virtual void handler_alr(OpCodeInfo* oci) = 0;
   virtual void handler_ahx(OpCodeInfo* oci) = 0;
   virtual void handler_sei(OpCodeInfo* oci) = 0;
   virtual void handler_iny(OpCodeInfo* oci) = 0;
   virtual void handler_inc(OpCodeInfo* oci) = 0;
   virtual void handler_cli(OpCodeInfo* oci) = 0;
   virtual void handler_beq(OpCodeInfo* oci) = 0;
   virtual void handler_cpy(OpCodeInfo* oci) = 0;
   virtual void handler_cld(OpCodeInfo* oci) = 0;
   virtual void handler_txs(OpCodeInfo* oci) = 0;
   virtual void handler_tas(OpCodeInfo* oci) = 0;
   virtual void handler_clc(OpCodeInfo* oci) = 0;
   virtual void handler_adc(OpCodeInfo* oci) = 0;
   virtual void handler_tsx(OpCodeInfo* oci) = 0;
   virtual void handler_xaa(OpCodeInfo* oci) = 0;
   virtual void handler_clv(OpCodeInfo* oci) = 0;
   virtual void handler_asl(OpCodeInfo* oci) = 0;
   virtual void handler_jmp(OpCodeInfo* oci) = 0;
   virtual void handler_bne(OpCodeInfo* oci) = 0;
   virtual void handler_ldy(OpCodeInfo* oci) = 0;
   virtual void handler_axs(OpCodeInfo* oci) = 0;
   virtual void handler_plp(OpCodeInfo* oci) = 0;
   virtual void handler_tax(OpCodeInfo* oci) = 0;
   virtual void handler_pha(OpCodeInfo* oci) = 0;
   virtual void handler_bmi(OpCodeInfo* oci) = 0;
   virtual void handler_rla(OpCodeInfo* oci) = 0;
   virtual void handler_tya(OpCodeInfo* oci) = 0;
   virtual void handler_tay(OpCodeInfo* oci) = 0;
   virtual void handler_sbc(OpCodeInfo* oci) = 0;
   virtual void handler_lax(OpCodeInfo* oci) = 0;
   virtual void handler_txa(OpCodeInfo* oci) = 0;
   virtual void handler_jsr(OpCodeInfo* oci) = 0;
   virtual void handler_kil(OpCodeInfo* oci) = 0;
   virtual void handler_bit(OpCodeInfo* oci) = 0;
   virtual void handler_php(OpCodeInfo* oci) = 0;
   virtual void handler_nop(OpCodeInfo* oci) = 0;
   virtual void handler_dcp(OpCodeInfo* oci) = 0;
   virtual void handler_ora(OpCodeInfo* oci) = 0;
   virtual void handler_dex(OpCodeInfo* oci) = 0;
   virtual void handler_dey(OpCodeInfo* oci) = 0;
   virtual void handler_dec(OpCodeInfo* oci) = 0;
   virtual void handler_sed(OpCodeInfo* oci) = 0;
   virtual void handler_sta(OpCodeInfo* oci) = 0;
   virtual void handler_sre(OpCodeInfo* oci) = 0;
   virtual void handler_shx(OpCodeInfo* oci) = 0;
   virtual void handler_shy(OpCodeInfo* oci) = 0;
   virtual void handler_bpl(OpCodeInfo* oci) = 0;
   virtual void handler_bcc(OpCodeInfo* oci) = 0;
   virtual void handler_cpx(OpCodeInfo* oci) = 0;
   virtual void handler_eor(OpCodeInfo* oci) = 0;
   virtual void handler_lda(OpCodeInfo* oci) = 0;
   virtual void handler_slo(OpCodeInfo* oci) = 0;
   virtual void handler_las(OpCodeInfo* oci) = 0;
   virtual void handler_isc(OpCodeInfo* oci) = 0;
   virtual void handler_brk(OpCodeInfo* oci) = 0;
   virtual void handler_cmp(OpCodeInfo* oci) = 0;
   virtual void handler_stx(OpCodeInfo* oci) = 0;
   virtual void handler_sty(OpCodeInfo* oci) = 0;
   virtual void handler_bcs(OpCodeInfo* oci) = 0;
   // End of auto generated code

protected:

   /**
    * Decodes instruction
    * @li Get operand bytes
    * @li Calling prehook handler
    * @li Calling opcode handler function
    * @li Calling posthook handler
    * @return Number of clock cycles instruction took, or -1 if there was an error
    */
   virtual int decode();

   virtual void updatePc(uint8_t bytesIncrement) = 0;

   /**
    * Doesn't do anything
    */
   virtual void preHandlerHook(OpCodeInfo* oci);

   /**
    * Post decoder operations hook.
    * @param oci
    * @return Number of clock cycles if emulating, -1 on error / halt
    */
   virtual int postHandlerHook(OpCodeInfo* oci);

   /// When decoding, these are the extra bytes of an op code (not always used)
   uint8_t theOpCode2;
   uint8_t theOpCode3;

};

#endif // DECODER6502_H
