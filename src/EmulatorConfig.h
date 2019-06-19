#ifndef EMULATOR_CONFIG_H
#define EMULATOR_CONFIG_H

/**
 * This file contains the configuration compiler directives all in a single
 * file.  This should make it easy to see what the different configurations
 * are
 */

// Turns on debug output in the UartDevice class
#define UART_TRACE

// This should be defined if SDL is available.  This flag is useful if there is
// a binary that needs to be build that doesn't require SDL / SDL features

// This config is currently set in the CMakeLists.txt
// #define SDL_INCLUDED

// Prints all CPU instructions executed to a trace.txt log
#define TRACE_EXECUTION                                                        

// Turns on debug statements in Cpu6502 class
#define CPU_TRACE                                                              

// Turns on debug statements about the address mode in the Cpu6502 class
// #define ADDR_MODE_TRACE                                                           

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

// Turns on the debug statements in the MemoryConfig class
#define CONFIG_TRACE

// Turns on debug statements in the MirrorMemory class
#define MIRROR_MEM_TRACE

// Turns on the debug statements in the RomMemory class
#define ROM_TRACE

// Turns on debug statements in the RamMemory class
// #define RAM_TRACE                                                

#endif

