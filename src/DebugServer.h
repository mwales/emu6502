#ifndef DEBUG_SERVER_H
#define DEBUG_SERVER_H

#include <stdint.h>
#include <SDL2/SDL_net.h>
#include <vector>

class Cpu6502;
class MemoryController;

#define DEBUGGER_MAX_MSG_LEN  2048

class DebugServer
{
public:
   DebugServer(Cpu6502* cpu, uint16_t portNum, MemoryController* memController);

   ~DebugServer();

   /// Called every CPU cycle to see if the debugger needs to do anything
   void debugHook();

   bool startDebugServer();

   static int debugServerThreadEntry(void* debuggerInstance);

   void emulatorHalt();


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
    * @param buffer Frame data
    */
   void sendResponse(int msgLen, uint8_t const * const buffer);

   void processCommand(uint16_t commandLen, uint16_t command);

   void versionCommand();

   void quitCommand();

   void disassembleCommand(uint16_t commandLen);

   Cpu6502* theCpu;

   MemoryController* theMemoryController;

   uint16_t thePortNumber;

   TCPsocket theServerSocket;

   SDL_Thread* theServerThread;

   //SDL_Thread* theClientMgr;

   TCPsocket theClientSocket;

   bool theRunningFlag;

   int theNumberBytesToRx;

   uint8_t theRxDataBuffer[DEBUGGER_MAX_MSG_LEN];

   SDLNet_SocketSet theSocketSet;

   bool theDebuggerBlockEmulatorFlag;

};

#endif // DEBUG_SERVER_H
