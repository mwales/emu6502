# emu6502

Had fun creating a [chip 8 emulator](https://github.com/mwales/chip8).  Going to try to create a
6502 emulator as a stepping stone towards creating 32-bit ARM emulator.

## 6502 Based Systems

* Apple IIe - I wonder if I still have a floppy disk around here of my 8th and 9th grade Apple IIe
  projects I made in computer class.  The Apple II was pretty cool because it was pretty easy to
  use the graphical modes from within BASIC.
* Atari 2600 - Wizards worked on this thing.   David Crane (creator of Pitfall and A Boy and his
  Blob did a [fantastic talk (GDC 2011 Pitfall Classic Postmortem with David Crane)
  ](https://www.youtube.com/watch?v=MBT1OK6VAIU) about nature of programming one of the consoles
  that started home video game revolution.
* Atari 800 Family - My older brothers first computer.  One of my first Christmas memories was when
  Santa Claus brought him a floppy drive.  I was too young at the time to understand the importance
  of the floppy drive vs tape drive.  Many memories playing Decathalon, M.U.L.E., and Racing
  Destruction Set.  My first programming was done here using Pilot.
* Commodore 64 Family - My first computer!!!  I wanted a NES, but parents were having none of this
  game machine crap in our home.  I wrote so many horrible BASIC programs on this.  I only had a 
  B/W TV connected to it for years, so never programmed using any colors.  My SID broke after a few
  years.  But I learned about Sprites and Disk I/O from reading the [Commodore 64: Users's Guide
  ](http://www.commodore.ca/commodore-manuals/commodore-64-users-guide/), and wore out my 1541
  reading and writing my sprite data to floppy disk.  I didn't even know that a sister manual
  existed, OMG!!: [Commodore 64: Programmer's Reference Guide
  ](http://www.commodore.ca/commodore-manuals/commodore-64-users-guide/)
* Famicom / Nintendo Entertainment System - Legendary start for Zelda, Super Mario Bros., Metroid, 
  Final Fantasy, and countless others.  I never owned one until recently.

## Design

* MemoryController: Class will manage switching out which MemoryDev class will be available at the
  6502 memory space.  For instance, C64 has special ROM memory and RAM that occupy the same memory
  address space depending on the current system mode.
* MemoryDev: Abstract class that defines an interface to read and write the many different memory
  mapped peripherals available to the 6502
  * RAM: Memory module representing the system RAM
  * ROM: Memory module representing the system ROM (For C64 this will include the kernel, chargen,
   and basic interpreter.
  * Interface: C64 and NES both have interface peripheral memory spaces to talk to things like 
   controllers or serial ports.
  * SID: C64 Sound Chip.  Probably one of the last things I will try to emulate, if I do ever,
   because I know so little about how sound ICs work on legacy or current devices.
  * VIC2: C64 video processor.  This will be connected to the user display.  I'm kind of
   wondering if it is possible to make this ncurses based too for plain text operation modes.
  * 6510: Special memory mapped features of the individual processors themselves.  I think the
   C64 CPU has a page or two of special memory mapped registers.
* Decoder: An abstract class for decoding instructions
  * Processor: Does the instruction decoding, and actually emulates the processor operations.
  * Disassembler: Does the instruction decoding, and makes a dissassembly listing of all the
   instructions.

## 6502 Stuff

### Registers

* A = accumulator
* X = general purpose register 1
* Y = general purpose register 2
* SP= stack pointer (can also think of it as 01SP since stack is on page 1
* PC= 16-bit program counter
* Flags
  * 0x80 Sign (N)
  * 0x40 Overflow (V)
  * 0x10 Breakpoint (B)
  * 0x08 Binary Coded Decimal (D)
  * 0x04 Interrupt (I)
  * 0x02 Zero (Z)
  * 0x01 Carry (C)

### Instructions

| MNM | Instruction        | Modes                   | Notes         |
|-----|--------------------|-------------------------|---------------|
| LDx | Load               |                         | A, X, or Y    |
| STx | Store              |                         | A, X, or Y    |
| Txx | Transfer           |                         | A->XY, X->AS, Y->A, S->X |
| ADC | Add with Carry     |                         |               |
| SBC | Sub with Carry     |                         |               |
| INx | Increment          |                         | X, Y, or Memory |
| DEx | Decrement          |                         | X, Y, or Memory |
| ORA | Or                 |                         |               |
| AND | And                |                         |               |
| EOR | Xor                |                         |               |
| ASL | Arithmetic Shift L |                         | Shift Left    |
| LSR | Logical Shift Rght |                         | Shift Right   |
| ROR | Rotate Right       |                         |               |
| ROL | Rotate Left        |                         |               |
| PHx | Push               |                         | Push A or (P)rocessor Flags onto stack |
| PLx | Pull               |                         | Pull A or (P)rocessor Flags from stack |
| JMP | Jump               |                         |               |
| JSR | Jump Subroutine    |                         |               |
| RTI | Return from Intrpt |                         |               |
| RTS | Return from Subrtn |                         |               |
| BIT | Bit Test           |                         | Status Flags  |
| CMP | Compare            |                         | A, X, or Y    |
| CLx | Clear              |                         | Clear processor status flag: (C)arry, (D)ecimal, (I)nterrupt disable, o(V)erflow |
| SEx | Set                |                         | Set processor status flag: (C)arry, (D)ecimal, (I)nterrupt disable |
| NOP | No Op              |                         |               | 
| BRK | Break              |                         | Halts and prints out registers |
| Bxx | Branch             |                         | (C)arry (C)lear, (C)arry (S)et, (EQ)ual or zero flag, (MI)nus, (N)ot (E)qual, (PL)us |
