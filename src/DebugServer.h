#ifndef DEBUG_SERVER_H
#define DEBUG_SERVER_H

#include <stdint.h>
#include <SDL2/SDL_net.h>
#include <vector>

class Cpu6502;

#define DEBUGGER_MAX_MSG_LEN  2048

class DebugServer
{
public:
   DebugServer(Cpu6502* cpu, uint16_t portNum);

   ~DebugServer();

   /// Called every CPU cycle to see if the debugger needs to do anything
   void debugHook();

   bool startDebugServer();

   static int debugServerThreadEntry(void* debuggerInstance);

protected:

   void closeExistingConnection(char const * reason);

   int debugServerSocketThread();

   Cpu6502* theCpu;

   uint16_t thePortNumber;

   TCPsocket theServerSocket;

   SDL_Thread* theServerThread;

   //SDL_Thread* theClientMgr;

   TCPsocket theClientSocket;

   bool theRunningFlag;

   int theNumberBytesToRx;

   uint8_t theRxDataBuffer[DEBUGGER_MAX_MSG_LEN];

   SDLNet_SocketSet theSocketSet;



};

#endif // DEBUG_SERVER_H
