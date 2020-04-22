#ifndef EMULATOR_CONFIG_H
#define EMULATOR_CONFIG_H

/**
 * This file contains the configuration compiler directives all in a single
 * file.  This should make it easy to see what the different configurations
 * are
 */

// Turns on debug for the configuration manager
// #define CONFIG_MANAGER_DEBUG

// Turns on debug output in the UartDevice class
// #define UART_TRACE

// This should be defined if SDL is available.  This flag is useful if there is
// a binary that needs to be build that doesn't require SDL / SDL features

// This config is currently set in the CMakeLists.txt
// #define SDL_INCLUDED

// Prints all CPU instructions executed to a trace.txt log
#define TRACE_EXECUTION

// Dumps all of memory into a .dump file when emulator closes
#define DUMP_MEMORY

// Turns on debug statements in Cpu6502 class
#define CPU_TRACE

// Turns on debug statements about the address mode in the Cpu6502 class
#define ADDR_MODE_TRACE

// Turns on debugger statements for the Decoder6502 class
#define DECODER_6502_DEBUG

// Turning this on implements the unofficial NES op-codes in Cpu6502 class
#define UNOFFICIAL_NES_OPCODE_SUPPORT                         

// Debugger state tracing from the DebuggerState class
// #define DSTATE_DEBUG_LOGGING

// Turns on debug statements from the DebugServer class
// #define DEBUG_SERVER_DEBUG

// Turns on debug statements from the Disassembler6502 class
// #define DISASS_TRACE
            
// Turns on debug statements for the Mapper class (and child mapper classes)
#define MAPPER_TRACE

// Turns on debug statements in the MirrorMemory class
#define MIRROR_MEM_TRACE

// Turns on the debug statements in the RomMemory and NesRom class
#define ROM_TRACE

// Turns on debug statements in the RamMemory class
// #define RAM_TRACE

// Turns on debug for the random number generator device
// #define RANDOMDEV_TRACE

// Turns on debug for the simple queue (for display commands)
// #define SIMPLE_QUEUE_LOGGING

// Turns on debug for the display manager
#define DISPLAY_MANAGER_TRACE

// Turns on debug statements for the display class
#define DISPLAYWINDOW_DEBUG

// Spits out a trace of all the SDL commands
// #define SDL_TRACE_DEBUG

// Turns on debug for the display device for the Easy6502 Javascript emulator
// #define EASY6502_DISPLAY_TRACE

// Turns on the debug for the input device for Easy6502 Javascript emulator
// #define EASY6502_INPUTDEV_TRACE

// Turns on the debug for the NES PPU Device
#define PPUDEV_TRACE

#endif

