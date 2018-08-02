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

### Addressing Modes

1. Immediate (2-byte instruction).  1-byte operand built right into the op-code.  Example: LDA #$ff
2. Absolute (3-byte instruction).  2-byte operand specifies the memory address.  Example: LDA $0200
3. Absolute Zero-Page (2-byte instruction).  1-byte operand specifies the offset within the zero
   page for the memory address.  Example: LDA $02
4. Implied (1-byte instruction). There is no operand.  Example: BRK
5. Accumulator (1-byte instruction). No operands, the accumulator is the implied operand.
   Example ROL A
6. Indexed (3-byte instruction).  2-byte operand is the memory location with offset provided by a
   register. Example: LDA $1234,X ; Loads accumulator with data from address 0x1234+X.
7. Indexed Zero-paged (2-byte instruction). 1-byte operand is the zero-page memory location with
   offset provided by a register.  Example: LDA $12,X ; Loads accumulator with data from address
   0x0012+X.
8. Indirect (3-byte instruction).  2-byte operand is loaded from an address specified by the
   address provided in the instruction. Example: JMP ($1234) ; Means to jump to the address found
   at the address $1234.  JMP is the only instruction that has this mode.
9. Relative (2-byte instruction).  1-byte operand is a signed offset from current PC.  Example:
   BEQ *+4 ; If equal branch to instruction PC += 4
10. Indirect Indexed (2-byte instruction). 1-byte operand is the address on zero page of a table
    that will be indexed by value in register.  Wraps around on zero page if value overflows.
    Example: LDA ($40,X) ; Loads a value from memory address specified in memory location 0x40+X
11. Indexed Indirect (2-byte instruction). 1-byte operand is the offset from address read from the
    zero-page address specified in the op-code.  Example: LDA ($40),X ; Loads a memory address from
    address 0x0040, then adds X, then reads the memory address into accumulator.

## Build Instructions

Build system is based on CMake. To build the emulator and disassembler:

```
mkdir build
cd build
cmake ../
make -j8
```

## Disassembler

This program can disassemble a single 64K ROM Image.  The filename of the binary and the base
address of the binary are required.  The emulator assumes that the base address is a code
entry point for the recursive disassembler.

Other entry points can be manually specified using the --address (-a) option to force the
disassembler to down different branches that it didn't detect as code.  These dead branches may
be because they were interrupt handler or jumped to via an indirect jump instruction.

The user can also specify if opcodes and memory addresses should be output as part of the
disassembly by using the -o option to enable opcodes, and the -l option to enable location
information.

### Usage Documenation

```
./dis6502-f filename -b baseaddr [-a address] [-h] [-o] [-l]
  -f --file       Filename of the file to disassemble
  -b --baseaddr   Base address of the file to disassemble
  -a --address    Address of extra entry points to recursively disassemble (multiple)
  -h --help       Show this help
  -o --opcodes    Show opcodes in the disassembly
  -l --location   Show locations / memory address of instructions and data in the listing
```

### Test Disassembler

Assemble the test assembly file in the src/test folder.  The crasm 6502 assembler is required for
this.  The assemble shell script will assemble the provided test file at the base address 0x4000.

To disassemble:

```
./dis6502 --file test/opCodes.bin --b 0x4000 -a 0x414c -o -l
```

## Emulator

### Debugger design

I don't want to clutter up the emulator with the debugger / menu system.  I
think I'm going to make it a remote control system over a socket, then I can
create a proper Qt based GUI for the debugger.  This should probably also
help with making a machine learning type of system for the emulator as well.

Commands for the emulator's debugger

* 0x01 About - Emulator returns a string about it's version (Basically a test / 
       heartbeat message
* 0x02 Exit - Tells the emulator to shutdown
* 0x03 List(flags, address, numInstruction) - Disassemble some instructions. Flag 0x1 indicates
     the address is provided.  0x02 indicates the numInstructions are provided.  flags is 8bit, 
     address is sizeof address, numinstructions is 16-bit
* 0x04 RegisterDump() - Dump all the registers.  X, Y, Accumulator, Status, PC, SP
* 0x05 Step(numInstructions) - Steps the emulator a finite number of instructions (16-bit)
* 0x06 Halt
* 0x07 Continue
* 0x08 MemoryRead(address, numBytes) - Address is CpuAddress size, numBytes is 16-bit.  Returns
     address (CpuAddress size), num bytes (16-bits), then raw binary data
* RegisterWrite(registerName, registerValue)
* MemoryWrite(address, numBytes, data)
* Breakpoint(address)
* BreakpointRemove(addresss)
* BreakpointList
* SaveState(filename) - Prefix RAM indicates save in memory based dictionary
* LoadState(filename) - Prefix RAM indicates save in memory based dictionary
* MemDev(ioctl, numBytes, dataBuffer) - Who knows what commands we will need these
     these things to support

