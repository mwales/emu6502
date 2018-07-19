#include "Cpu6502.h"
#include "Logger.h"
#include "Utils.h"



Cpu6502::Cpu6502()
{

}

void Cpu6502::start(CpuAddress address)
{

}

void Cpu6502::updatePc(uint8_t bytesIncrement)
{

}




/***   CREATING HANDLER BLANK FUNCTIONS ***/
void Cpu6502::handler_and(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for and";
}

void Cpu6502::handler_bvs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bvs";
}

void Cpu6502::handler_sec(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sec";
}

void Cpu6502::handler_rol(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rol";
}

void Cpu6502::handler_pla(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for pla";
}

void Cpu6502::handler_anc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for anc";
}

void Cpu6502::handler_rti(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rti";
}

void Cpu6502::handler_arr(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for arr";
}

void Cpu6502::handler_rra(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rra";
}

void Cpu6502::handler_bvc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bvc";
}

void Cpu6502::handler_sax(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sax";
}

void Cpu6502::handler_lsr(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for lsr";
}

void Cpu6502::handler_rts(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rts";
}

void Cpu6502::handler_inx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for inx";
}

void Cpu6502::handler_ror(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ror";
}

void Cpu6502::handler_ldx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ldx";
}

void Cpu6502::handler_alr(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for alr";
}

void Cpu6502::handler_ahx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ahx";
}

void Cpu6502::handler_sei(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sei";
}

void Cpu6502::handler_iny(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for iny";
}

void Cpu6502::handler_inc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for inc";
}

void Cpu6502::handler_cli(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cli";
}

void Cpu6502::handler_beq(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for beq";
}

void Cpu6502::handler_cpy(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cpy";
}

void Cpu6502::handler_cld(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cld";
}

void Cpu6502::handler_txs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for txs";
}

void Cpu6502::handler_tas(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tas";
}

void Cpu6502::handler_clc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for clc";
}

void Cpu6502::handler_adc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for adc";
}

void Cpu6502::handler_tsx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tsx";
}

void Cpu6502::handler_xaa(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for xaa";
}

void Cpu6502::handler_clv(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for clv";
}

void Cpu6502::handler_asl(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for asl";
}

void Cpu6502::handler_jmp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for jmp";
}

void Cpu6502::handler_bne(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bne";
}

void Cpu6502::handler_ldy(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ldy";
}

void Cpu6502::handler_axs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for axs";
}

void Cpu6502::handler_plp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for plp";
}

void Cpu6502::handler_tax(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tax";
}

void Cpu6502::handler_pha(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for pha";
}

void Cpu6502::handler_bmi(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bmi";
}

void Cpu6502::handler_rla(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for rla";
}

void Cpu6502::handler_tya(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tya";
}

void Cpu6502::handler_tay(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for tay";
}

void Cpu6502::handler_sbc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sbc";
}

void Cpu6502::handler_lax(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for lax";
}

void Cpu6502::handler_txa(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for txa";
}

void Cpu6502::handler_jsr(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for jsr";
}

void Cpu6502::handler_kil(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for kil";
}

void Cpu6502::handler_bit(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bit";
}

void Cpu6502::handler_php(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for php";
}

void Cpu6502::handler_nop(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for nop";
}

void Cpu6502::handler_dcp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dcp";
}

void Cpu6502::handler_ora(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for ora";
}

void Cpu6502::handler_dex(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dex";
}

void Cpu6502::handler_dey(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dey";
}

void Cpu6502::handler_dec(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for dec";
}

void Cpu6502::handler_sed(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sed";
}

void Cpu6502::handler_sta(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sta";
}

void Cpu6502::handler_sre(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sre";
}

void Cpu6502::handler_shx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for shx";
}

void Cpu6502::handler_shy(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for shy";
}

void Cpu6502::handler_bpl(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bpl";
}

void Cpu6502::handler_bcc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bcc";
}

void Cpu6502::handler_cpx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cpx";
}

void Cpu6502::handler_eor(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for eor";
}

void Cpu6502::handler_lda(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for lda";
}

void Cpu6502::handler_slo(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for slo";
}

void Cpu6502::handler_las(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for las";
}

void Cpu6502::handler_isc(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for isc";
}

void Cpu6502::handler_brk(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for brk";
}

void Cpu6502::handler_cmp(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for cmp";
}

void Cpu6502::handler_stx(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for stx";
}

void Cpu6502::handler_sty(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for sty";
}

void Cpu6502::handler_bcs(OpCodeInfo* oci)
{
   LOG_DEBUG() << "Empty handler for bcs";
}





