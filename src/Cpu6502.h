#ifndef CPU6502_H
#define CPU6502_H

#include "Decoder6502.h"

class Cpu6502 : public Decoder6502
{
public:
    Cpu6502();

    virtual void start(CpuAddress address);

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

protected:

    virtual void updatePc(uint8_t bytesIncrement);

    uint8_t theRegX;
    uint8_t theRegY;

    CpuAddress theStackPointer;



};

#endif // CPU6502_H
