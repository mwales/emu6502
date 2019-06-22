#ifndef DEBUG_SERVER_H
#define DEBUG_SERVER_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <vector>
#include <set>
#include "DebuggerState.h"
#include "Cpu6502Defines.h"

class Cpu6502;
class MemoryController;

#define DEBUGGER_MAX_MSG_LEN  2048

/**
 * Class that the debugger client connects to via TCP connection to command the debugger
 */
class DebugServer
{
public:
   DebugServer(Cpu6502* cpu, uint16_t portNum, MemoryController* memController);

   ~DebugServer();

   /// Called every CPU cycle to see if the debugger needs to do anything
   void debugHook();

   /**
    * Called by the emulator when memory address are accessed for reading or writing
    * @param addr Memory address being written
    * @param isWrite If the access was a read or write
    */
   void debugMemoryAccessHook(CpuAddress addr, bool isWrite);

   bool startDebugServer();

   static int debugServerThreadEntry(void* debuggerInstance);

   static void emulatorHalt(void* thisPtr);


protected:

   void closeExistingConnection(char const * reason);

   int debugServerSocketThread();

   /**
    * Sends a framed message to the debugger client.  The frame contains a 2-byte message length
    * field on the front of the message
    *
    * @note The message length sent over the wire doesn't account for 2 bytes of the frame header
    *
    * @param msgLen Number of bytes in the frame
    * @param buffer Frame data.  Will be free-ed before returning
    */
   void sendResponse(int msgLen, uint8_t const * const buffer);

   void processCommand(uint16_t commandLen, uint16_t command);

   void versionCommand();

   void quitCommand();

   void disassembleCommand(uint16_t commandLen);

   void dumpRegistersCommand();

   void stepCommand(uint16_t commandLen);

   void continueCommand();

   void haltCommand();

   void memoryDumpCommand(uint16_t commandLen);

   void addBreakpointCommand(uint16_t command, uint16_t commandLen);
   void removeBreakpointCommand(uint16_t command, uint16_t commandLen);
   void listBreakpointCommand();

   /**
    * Sends the list of breakpoints to the debugger client.  Breakpoint list format is:
    * uint16_t Number of instruction breakpoints
    * uint16_t Number of memory access breakpoints
    * uint16_t[n] CPU Address of instructions breakpoints
    * uint16_t[n] CPU Address of memory access breakpoints
    */
   void sendBreakpointList();

   Cpu6502* theCpu;

   MemoryController* theMemoryController;

   uint16_t thePortNumber;

   TCPsocket theServerSocket;

   SDL_Thread* theServerThread;

   TCPsocket theClientSocket;

   /// This is set to zero when the debugger needs to exit, application closing
   bool theRunningFlag;

   int theNumberBytesToRx;

   uint8_t theRxDataBuffer[DEBUGGER_MAX_MSG_LEN];

   SDLNet_SocketSet theSocketSet;

   /// This is given by emulator thread to let the debugger know the emulator halted
   SDL_sem* theEmulatorHaltedSem;

   DebuggerState theDebuggerState;

   /// Stepping into an emulator fault will try to send 2 dumps to the client
   bool theRegisterDumpSentToClient;

   std::set<CpuAddress> theBreakpoints;

   std::set<CpuAddress> theMemoryAccessBPs;

};

#endif // DEBUG_SERVER_H
