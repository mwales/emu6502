#include "Cpu6502Defines.h"
#include "Decoder6502.h"

OpCodeInfo gOpCodes[0xff];

void constructCpuGlobals()
{

   CPU_OP( 0x0, 1, BRK, brk, IMPLIED    , 0, false, FLG_INTD | FLG_DECI)
   CPU_OP( 0x1, 2, ORA, ora, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP( 0x2, 1, KIL, kil, IMPLIED    , 1, false, FLG_NONE)
   CPU_OP( 0x3, 2, SLO, slo, INDIRECT_X , 8, false, FLG_NONE)
   CPU_OP( 0x4, 2, NOP, nop, ZERO_PAGE  , 3, false, FLG_ZERO)
   CPU_OP( 0x5, 2, ORA, ora, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP( 0x6, 2, ASL, asl, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP( 0x7, 2, SLO, slo, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP( 0x8, 1, PHP, php, IMPLIED    , 3, false, FLG_NONE)
   CPU_OP( 0x9, 2, ORA, ora, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP( 0xa, 1, ASL, asl, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP( 0xb, 2, ANC, anc, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP( 0xc, 3, NOP, nop, ABSOLUTE   , 4, false, FLG_ZERO)
   CPU_OP( 0xd, 3, ORA, ora, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP( 0xe, 3, ASL, asl, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP( 0xf, 3, SLO, slo, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x10, 2, BPL, bpl, RELATIVE   , 3, false, FLG_NONE)
   CPU_OP(0x11, 2, ORA, ora, INDIRECT_Y , 5, false, FLG_NONE)
   CPU_OP(0x12, 1, KIL, kil, IMPLIED    , 1, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0x13, 2, SLO, slo, INDIRECT_Y , 8, false, FLG_NONE)
   CPU_OP(0x14, 2, NOP, nop, ZERO_PAGE_X, 4, false, FLG_ZERO)
   CPU_OP(0x15, 2, ORA, ora, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0x16, 2, ASL, asl, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x17, 2, SLO, slo, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x18, 1, CLC, clc, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x19, 3, ORA, ora, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0x1a, 1, NOP, nop, IMPLIED    , 2, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0x1b, 3, SLO, slo, ABSOLUTE_Y , 7, false, FLG_NONE)
   CPU_OP(0x1c, 3, NOP, nop, ABSOLUTE_X , 4, false, FLG_ZERO)
   CPU_OP(0x1d, 3, ORA, ora, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0x1e, 3, ASL, asl, ABSOLUTE_X , 7, true,  FLG_ZERO | FLG_NEG )
   CPU_OP(0x1f, 3, SLO, slo, ABSOLUTE_X , 7, false, FLG_NONE)
   CPU_OP(0x20, 3, JSR, jsr, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x21, 2, AND, and, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP(0x22, 1, KIL, kil, IMPLIED    , 1, false, FLG_NONE)
   CPU_OP(0x23, 2, RLA, rla, INDIRECT_X , 8, false, FLG_NONE)
   CPU_OP(0x24, 2, BIT, bit, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x25, 2, AND, and, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x26, 2, ROL, rol, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0x27, 2, RLA, rla, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0x28, 1, PLP, plp, IMPLIED    , 4, false, FLG_NONE)
   CPU_OP(0x29, 2, AND, and, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0x2a, 1, ROL, rol, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x2b, 2, ANC, anc, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0x2c, 3, BIT, bit, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0x2d, 3, AND, and, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0x2e, 3, ROL, rol, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x2f, 3, RLA, rla, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x30, 2, BMI, bmi, RELATIVE   , 2, false, FLG_NONE)
   CPU_OP(0x31, 2, AND, and, INDIRECT_Y , 5, false, FLG_NONE)
   CPU_OP(0x32, 1, KIL, kil, IMPLIED    , 1, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0x33, 2, RLA, rla, INDIRECT_Y , 8, false, FLG_NONE)
   CPU_OP(0x34, 2, NOP, nop, ZERO_PAGE_X, 4, false, FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x35, 2, AND, and, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0x36, 2, ROL, rol, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x37, 2, RLA, rla, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x38, 1, SEC, sec, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x39, 3, AND, and, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0x3a, 1, NOP, nop, IMPLIED    , 2, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0x3b, 3, RLA, rla, ABSOLUTE_Y , 7, false, FLG_NONE)
   CPU_OP(0x3c, 3, NOP, nop, ABSOLUTE_X , 4, true,  FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x3d, 3, AND, and, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0x3e, 3, ROL, rol, ABSOLUTE_X , 7, true,  FLG_ZERO | FLG_NEG )
   CPU_OP(0x3f, 3, RLA, rla, ABSOLUTE_X , 7, false, FLG_NONE)
   CPU_OP(0x40, 1, RTI, rti, IMPLIED    , 6, false, FLG_NONE)
   CPU_OP(0x41, 2, EOR, eor, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP(0x42, 1, KIL, kil, IMPLIED    , 1, false, FLG_NONE)
   CPU_OP(0x43, 2, SRE, sre, INDIRECT_X , 8, false, FLG_NONE)
   CPU_OP(0x44, 2, NOP, nop, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x45, 2, EOR, eor, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x46, 2, LSR, lsr, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0x47, 2, SRE, sre, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0x48, 1, PHA, pha, IMPLIED    , 3, false, FLG_NONE)
   CPU_OP(0x49, 2, EOR, eor, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0x4a, 1, LSR, lsr, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x4b, 2, ALR, alr, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0x4c, 3, JMP, jmp, ABSOLUTE   , 3, false, FLG_NONE)
   CPU_OP(0x4d, 3, EOR, eor, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0x4e, 3, LSR, lsr, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x4f, 3, SRE, sre, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x50, 2, BVC, bvc, RELATIVE   , 3, false, FLG_NONE)
   CPU_OP(0x51, 2, EOR, eor, INDIRECT_Y , 5, false, FLG_NONE)
   CPU_OP(0x52, 1, KIL, kil, IMPLIED    , 1, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0x53, 2, SRE, sre, INDIRECT_Y , 8, false, FLG_NONE)
   CPU_OP(0x54, 2, NOP, nop, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0x55, 2, EOR, eor, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0x56, 2, LSR, lsr, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x57, 2, SRE, sre, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x58, 1, CLI, cli, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x59, 3, EOR, eor, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0x5a, 1, NOP, nop, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x5b, 3, SRE, sre, ABSOLUTE_Y , 7, false, FLG_NONE)
   CPU_OP(0x5c, 3, NOP, nop, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0x5d, 3, EOR, eor, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0x5e, 3, LSR, lsr, ABSOLUTE_X , 7, true,  FLG_ZERO | FLG_NEG )
   CPU_OP(0x5f, 3, SRE, sre, ABSOLUTE_X , 7, false, FLG_NONE)
   CPU_OP(0x60, 1, RTS, rts, IMPLIED    , 6, false, FLG_NONE)
   CPU_OP(0x61, 2, ADC, adc, INDIRECT_X , 6, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x62, 1, KIL, kil, IMPLIED    , 1, false, FLG_NONE)
   CPU_OP(0x63, 2, RRA, rra, INDIRECT_X , 8, false, FLG_NONE)
   CPU_OP(0x64, 2, NOP, nop, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x65, 2, ADC, adc, ZERO_PAGE  , 3, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x66, 2, ROR, ror, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0x67, 2, RRA, rra, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0x68, 1, PLA, pla, IMPLIED    , 4, false, FLG_NONE)
   CPU_OP(0x69, 2, ADC, adc, IMMEDIATE  , 2, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x6a, 1, ROR, ror, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x6b, 2, ARR, arr, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0x6c, 3, JMP, jmp, INDIRECT   , 5, false, FLG_NONE)
   CPU_OP(0x6d, 3, ADC, adc, ABSOLUTE   , 4, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x6e, 3, ROR, ror, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x6f, 3, RRA, rra, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0x70, 2, BVS, bvs, RELATIVE   , 2, false, FLG_NONE)
   CPU_OP(0x71, 2, ADC, adc, INDIRECT_Y , 5, false, FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x72, 1, KIL, kil, IMPLIED    , 1, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x73, 2, RRA, rra, INDIRECT_Y , 8, false, FLG_NONE)
   CPU_OP(0x74, 2, NOP, nop, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0x75, 2, ADC, adc, ZERO_PAGE_X, 4, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x76, 2, ROR, ror, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x77, 2, RRA, rra, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0x78, 1, SEI, sei, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x79, 3, ADC, adc, ABSOLUTE_Y , 4, false, FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x7a, 1, NOP, nop, IMPLIED    , 2, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0x7b, 3, RRA, rra, ABSOLUTE_Y , 7, false, FLG_NONE)
   CPU_OP(0x7c, 3, NOP, nop, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0x7d, 3, ADC, adc, ABSOLUTE_X , 4, false, FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0x7e, 3, ROR, ror, ABSOLUTE_X , 7, true,  FLG_ZERO | FLG_NEG )
   CPU_OP(0x7f, 3, RRA, rra, ABSOLUTE_X , 7, false, FLG_NONE)
   CPU_OP(0x80, 2, NOP, nop, IMMEDIATE  , 2, true,  FLG_NONE)
   CPU_OP(0x81, 2, STA, sta, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP(0x82, 2, NOP, nop, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0x83, 2, SAX, sax, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP(0x84, 2, STY, sty, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x85, 2, STA, sta, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x86, 2, STX, stx, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x87, 2, SAX, sax, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0x88, 1, DEY, dey, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x89, 2, NOP, nop, IMMEDIATE  , 2, false, FLG_ZERO)
   CPU_OP(0x8a, 1, TXA, txa, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x8b, 2, XAA, xaa, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0x8c, 3, STY, sty, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0x8d, 3, STA, sta, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0x8e, 3, STX, stx, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0x8f, 3, SAX, sax, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0x90, 2, BCC, bcc, RELATIVE   , 3, false, FLG_NONE)
   CPU_OP(0x91, 2, STA, sta, INDIRECT_Y , 6, false, FLG_NONE)
   CPU_OP(0x92, 1, KIL, kil, IMPLIED    , 1, false, FLG_NONE)
   CPU_OP(0x93, 2, AHX, ahx, INDIRECT_Y , 6, false, FLG_NONE)
   CPU_OP(0x94, 2, STY, sty, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0x95, 2, STA, sta, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0x96, 2, STX, stx, ZERO_PAGE_Y, 4, false, FLG_NONE)
   CPU_OP(0x97, 2, SAX, sax, ZERO_PAGE_Y, 4, false, FLG_NONE)
   CPU_OP(0x98, 1, TYA, tya, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x99, 3, STA, sta, ABSOLUTE_Y , 5, false, FLG_NONE)
   CPU_OP(0x9a, 1, TXS, txs, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0x9b, 3, TAS, tas, ABSOLUTE_Y , 5, false, FLG_NONE)
   CPU_OP(0x9c, 3, SHY, shy, ABSOLUTE_X , 5, false, FLG_NONE)
   CPU_OP(0x9d, 3, STA, sta, ABSOLUTE_X , 5, false, FLG_NONE)
   CPU_OP(0x9e, 3, SHX, shx, ABSOLUTE_Y , 5, false, FLG_NONE)
   CPU_OP(0x9f, 3, AHX, ahx, ABSOLUTE_Y , 5, false, FLG_NONE)
   CPU_OP(0xa0, 2, LDY, ldy, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xa1, 2, LDA, lda, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP(0xa2, 2, LDX, ldx, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xa3, 2, LAX, lax, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP(0xa4, 2, LDY, ldy, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0xa5, 2, LDA, lda, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0xa6, 2, LDX, ldx, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0xa7, 2, LAX, lax, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0xa8, 1, TAY, tay, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xa9, 2, LDA, lda, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xaa, 1, TAX, tax, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xab, 2, LAX, lax, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xac, 3, LDY, ldy, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0xad, 3, LDA, lda, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0xae, 3, LDX, ldx, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0xaf, 3, LAX, lax, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0xb0, 2, BCS, bcs, RELATIVE   , 2, false, FLG_NONE)
   CPU_OP(0xb1, 2, LDA, lda, INDIRECT_Y , 5, false, FLG_NONE)
   CPU_OP(0xb2, 1, KIL, kil, IMPLIED    , 1, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0xb3, 2, LAX, lax, INDIRECT_Y , 5, false, FLG_NONE)
   CPU_OP(0xb4, 2, LDY, ldy, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0xb5, 2, LDA, lda, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0xb6, 2, LDX, ldx, ZERO_PAGE_Y, 4, false, FLG_NONE)
   CPU_OP(0xb7, 2, LAX, lax, ZERO_PAGE_Y, 4, false, FLG_NONE)
   CPU_OP(0xb8, 1, CLV, clv, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xb9, 3, LDA, lda, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0xba, 1, TSX, tsx, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xbb, 3, LAS, las, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0xbc, 3, LDY, ldy, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0xbd, 3, LDA, lda, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0xbe, 3, LDX, ldx, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0xbf, 3, LAX, lax, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0xc0, 2, CPY, cpy, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xc1, 2, CMP, cmp, INDIRECT_X , 6, false, FLG_NONE)
   CPU_OP(0xc2, 2, NOP, nop, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xc3, 2, DCP, dcp, INDIRECT_X , 8, false, FLG_NONE)
   CPU_OP(0xc4, 2, CPY, cpy, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0xc5, 2, CMP, cmp, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0xc6, 2, DEC, dec, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0xc7, 2, DCP, dcp, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0xc8, 1, INY, iny, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xc9, 2, CMP, cmp, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xca, 1, DEX, dex, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xcb, 2, AXS, axs, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xcc, 3, CPY, cpy, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0xcd, 3, CMP, cmp, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0xce, 3, DEC, dec, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0xcf, 3, DCP, dcp, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0xd0, 2, BNE, bne, RELATIVE   , 3, false, FLG_NONE)
   CPU_OP(0xd1, 2, CMP, cmp, INDIRECT_Y , 5, false, FLG_NONE)
   CPU_OP(0xd2, 1, KIL, kil, IMPLIED    , 1, false, FLG_CARY | FLG_ZERO | FLG_NEG )
   CPU_OP(0xd3, 2, DCP, dcp, INDIRECT_Y , 8, false, FLG_NONE)
   CPU_OP(0xd4, 2, NOP, nop, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0xd5, 2, CMP, cmp, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0xd6, 2, DEC, dec, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0xd7, 2, DCP, dcp, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0xd8, 1, CLD, cld, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xd9, 3, CMP, cmp, ABSOLUTE_Y , 4, false, FLG_NONE)
   CPU_OP(0xda, 1, NOP, nop, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xdb, 3, DCP, dcp, ABSOLUTE_Y , 7, false, FLG_NONE)
   CPU_OP(0xdc, 3, NOP, nop, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0xdd, 3, CMP, cmp, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0xde, 3, DEC, dec, ABSOLUTE_X , 7, false, FLG_NONE)
   CPU_OP(0xdf, 3, DCP, dcp, ABSOLUTE_X , 7, false, FLG_NONE)
   CPU_OP(0xe0, 2, CPX, cpx, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xe1, 2, SBC, sbc, INDIRECT_X , 6, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xe2, 2, NOP, nop, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xe3, 2, ISC, isc, INDIRECT_X , 8, false, FLG_NONE)
   CPU_OP(0xe4, 2, CPX, cpx, ZERO_PAGE  , 3, false, FLG_NONE)
   CPU_OP(0xe5, 2, SBC, sbc, ZERO_PAGE  , 3, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xe6, 2, INC, inc, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0xe7, 2, ISC, isc, ZERO_PAGE  , 5, false, FLG_NONE)
   CPU_OP(0xe8, 1, INX, inx, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xe9, 2, SBC, sbc, IMMEDIATE  , 2, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xea, 1, NOP, nop, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xeb, 2, SBC, sbc, IMMEDIATE  , 2, false, FLG_NONE)
   CPU_OP(0xec, 3, CPX, cpx, ABSOLUTE   , 4, false, FLG_NONE)
   CPU_OP(0xed, 3, SBC, sbc, ABSOLUTE   , 4, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xee, 3, INC, inc, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0xef, 3, ISC, isc, ABSOLUTE   , 6, false, FLG_NONE)
   CPU_OP(0xf0, 2, BEQ, beq, RELATIVE   , 2, false, FLG_NONE)
   CPU_OP(0xf1, 2, SBC, sbc, INDIRECT_Y , 5, false, FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xf2, 1, KIL, kil, IMPLIED    , 1, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xf3, 2, ISC, isc, INDIRECT_Y , 8, false, FLG_NONE)
   CPU_OP(0xf4, 2, NOP, nop, ZERO_PAGE_X, 4, false, FLG_NONE)
   CPU_OP(0xf5, 2, SBC, sbc, ZERO_PAGE_X, 4, true,  FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xf6, 2, INC, inc, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0xf7, 2, ISC, isc, ZERO_PAGE_X, 6, false, FLG_NONE)
   CPU_OP(0xf8, 1, SED, sed, IMPLIED    , 2, false, FLG_NONE)
   CPU_OP(0xf9, 3, SBC, sbc, ABSOLUTE_Y , 4, false, FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xfa, 1, NOP, nop, IMPLIED    , 2, false, FLG_ZERO | FLG_NEG )
   CPU_OP(0xfb, 3, ISC, isc, ABSOLUTE_Y , 7, false, FLG_NONE)
   CPU_OP(0xfc, 3, NOP, nop, ABSOLUTE_X , 4, false, FLG_NONE)
   CPU_OP(0xfd, 3, SBC, sbc, ABSOLUTE_X , 4, false, FLG_CARY | FLG_OVFL | FLG_ZERO | FLG_NEG )
   CPU_OP(0xfe, 3, INC, inc, ABSOLUTE_X , 7, false, FLG_NONE)
   CPU_OP(0xff, 3, ISC, isc, ABSOLUTE_X , 7, false, FLG_NONE)

}
