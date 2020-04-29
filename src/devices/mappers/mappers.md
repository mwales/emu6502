# NES Memory Map

CPU addressable memory

0x0000 - 0x07f0   NES RAM
0x0800 - 0x1fff      Mirror of NES RAM
0x2000 - 0x2007   PPU registers
0x2008 - 0x3fff      PPU mirror
0x4000 - 0x4017   NES APU and I/O Registers
0x4018 - 0x401f      Test mode functionality
0x4020 - 0xffff   Cartridge Memory

PPU also has it's own memory space that is separate.  This memory space is
16 KB (0x0000 - 0x3fff).  The CHR portion of NES ROMs are mapped to these
addresses.

## Interrupt Vecotrs

0xfffa - 0xfffb   NMI vector
0xfffc - 0xfffd   Reset vector
0xfffe - 0xffff   IRQ / BRK vector

## iNES Format

The file format for iNES ROMS is:

* iNES header (16 bytes)
* Trainer (0 - 512 bytes)
* PRG ROM
* CHR ROM
* PlayChoice INST-ROM
* PlayChoice PROM

# Mappers Implemented

0x00 NROM


## NROM (0x00)

PRG ROM is 16 or 32 KB
PRG RAM is 2 or 4 KB
CHR ROM is 8 KB

0x6000 - 0x7fff   8KB  PRG RAM (most emus even treat ROM as RAM)
0x8000 - 0xbfff  16KB  
0xc000 - 0xffff  16KB  Last 16KB of ROM, or mirror
