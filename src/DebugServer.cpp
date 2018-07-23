#include "DebugServer.h"
#include "Logger.h"

DebugServer::DebugServer(Cpu6502* cpu, uint16_t portNum):
   theCpu(cpu),
   thePortNumber(portNum),
   theServerSocket(nullptr),
   theServerThread(nullptr),
   theClientSocket(nullptr),
   theRunningFlag(true),
   theNumberBytesToRx(-1)
{
   theSocketSet = SDLNet_AllocSocketSet(4);

   startDebugServer();
}

DebugServer::~DebugServer()
{
   // Tell the thread that we are exitting
   theRunningFlag = false;

   if (theServerThread != nullptr)
   {
      int status;
      SDL_WaitThread(theServerThread, &status);

      LOG_DEBUG() << "Debugger thread exitted with code " << status;
   }

   SDLNet_TCP_Close(theServerSocket);

   SDLNet_FreeSocketSet(theSocketSet);
}

bool DebugServer::startDebugServer()
{
   // Create socket
   IPaddress ip;
   if (SDLNet_ResolveHost(&ip, NULL, thePortNumber) != 0)
   {
      LOG_WARNING() << "ResolveHost call failed:" << SDLNet_GetError();
      return false;
   }

   theServerSocket = SDLNet_TCP_Open(&ip);
   if (!theServerSocket)
   {
      LOG_WARNING() << "TCP Open call failed:" << SDLNet_GetError();
      return false;
   }

   // Start the debugger thread
   theServerThread = SDL_CreateThread(DebugServer::debugServerThreadEntry, "T_debug", this);

   return true;
}

void DebugServer::debugHook()
{

}

int DebugServer::debugServerThreadEntry(void* debuggerInstance)
{
   DebugServer* instance = (DebugServer*) debuggerInstance;
   return instance->debugServerSocketThread();
}

int DebugServer::debugServerSocketThread()
{
   LOG_DEBUG() << "Starting thread to listen for connections!";

   while(theRunningFlag)
   {
      LOG_DEBUG() << "Debugger Loop Iteration Start";

      TCPsocket newConnection;
      newConnection = SDLNet_TCP_Accept(theServerSocket);

      if (newConnection)
      {
         // We just got a new connection!
         LOG_DEBUG() << "Accepted a new debugger connection";

         theNumberBytesToRx = -1;
         memset(theRxDataBuffer, 0, DEBUGGER_MAX_MSG_LEN);

         if (theClientSocket)
            closeExistingConnection("New connection");

         theClientSocket = newConnection;
         SDLNet_TCP_AddSocket(theSocketSet, theClientSocket);
      }

      // Any activities on the sockets?
      int activity = SDLNet_CheckSockets(theSocketSet, 1000);

      if (activity)
      {
         int numBytes = SDLNet_TCP_Recv(theClientSocket, theRxDataBuffer, 4);

         if (numBytes < 4)
         {
            if (numBytes <= 0)
            {
               closeExistingConnection("Error / Recv failed");
            }
            else
            {
               closeExistingConnection("Incomplete Header");
            }

            continue;
         }

         uint16_t numBytesFrame = SDLNet_Read16(theRxDataBuffer);
         uint16_t command = SDLNet_Read16(theRxDataBuffer + 2);

         LOG_DEBUG() << "Received a header.  Frame Size = " << numBytesFrame
                     << " and command = " << command;
      }

   }

   LOG_DEBUG() << "Debugger thread now exitting";

   return 0;
}


void DebugServer::closeExistingConnection(char const * reason)
{
   LOG_WARNING() << "Closing existing debugger client connection: " << reason;
   SDLNet_TCP_DelSocket(theSocketSet, theClientSocket);
   SDLNet_TCP_Close(theClientSocket);
   theClientSocket = nullptr;
}

