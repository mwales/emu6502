#include "DebugServer.h"
#include "Logger.h"
#include "Cpu6502.h"

DebugServer::DebugServer(Cpu6502* cpu, uint16_t portNum):
   theCpu(cpu),
   thePortNumber(portNum),
   theServerSocket(nullptr),
   theServerThread(nullptr),
   theClientSocket(nullptr),
   theRunningFlag(true),
   theNumberBytesToRx(-1),
   theDebuggerBlockEmulatorFlag(true)
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
   while (theDebuggerBlockEmulatorFlag)
   {
      LOG_DEBUG() << "Emulator blocked on debugger";
      SDL_Delay(5000);
   }

}

int DebugServer::debugServerThreadEntry(void* debuggerInstance)
{
   DebugServer* instance = (DebugServer*) debuggerInstance;
   return instance->debugServerSocketThread();
}

void DebugServer::emulatorHalt()
{

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

         int bytesReceived = 0;
         int bytesReceivedTotal = 0;
         while(bytesReceivedTotal < numBytesFrame)
         {
            bytesReceived = SDLNet_TCP_Recv(theClientSocket,
                                            theRxDataBuffer + bytesReceivedTotal,
                                            numBytesFrame - bytesReceivedTotal);

            if (bytesReceived <= 0)
            {
               LOG_WARNING() << "Error.  Only received " << bytesReceivedTotal
                             << "bytes of frame with " << numBytesFrame << " bytes";
               closeExistingConnection("Truncated frame");
               break;
            }

            bytesReceivedTotal += bytesReceived;
         }

         LOG_DEBUG() << "Received full frame of " << numBytesFrame << "bytes, cmd = "
                     << command;

         processCommand(numBytesFrame, command);


      }

   }

   LOG_DEBUG() << "Debugger thread now exitting";

   return 0;
}

void DebugServer::sendResponse(int msgLen, uint8_t const * const buffer)
{
   // We need a buffer larger the msgLen to hold the length value we need to send back
   uint8_t* txBuffer = (uint8_t*) malloc(msgLen + 2);
   SDLNet_Write16(msgLen, txBuffer);
   memcpy(txBuffer + 2, buffer, msgLen);

   // Send the buffer
   int bytesSent = SDLNet_TCP_Send(theClientSocket, txBuffer, msgLen + 2);

   if  (bytesSent != msgLen + 2)
   {
      LOG_WARNING() << "Failed to send response.  " << bytesSent << " bytes send of message length "
                    << (msgLen + 2);
      closeExistingConnection("Failed to send message to the client");
   }
   else
   {
      LOG_DEBUG() << "Sent a " << bytesSent << " byte message to the debugger client";
   }

   // Free the memory of the buffer we created
   free(txBuffer);
}

void DebugServer::processCommand(uint16_t commandLen, uint16_t command)
{
   switch(command)
   {
   case 1: // VERSION
      versionCommand();
      break;

   case 2: // QUIT
      quitCommand();
      break;

   default:
      LOG_WARNING() << "Command " << command << " is not implemented!";
   }
}

void DebugServer::versionCommand()
{
   LOG_DEBUG() << "Version command sent by the debugger client";

   char const * versionString = "6502 Emulator Version 0.0 (No System)";
   sendResponse(strlen(versionString), (uint8_t*) versionString);
}

void DebugServer::quitCommand()
{
   LOG_DEBUG() << "Exit emulator command sent by debugger client";
   theCpu->exitEmulation();

   // unblock the emulator so it can exit
   theDebuggerBlockEmulatorFlag = false;
}


void DebugServer::closeExistingConnection(char const * reason)
{
   LOG_WARNING() << "Closing existing debugger client connection: " << reason;
   SDLNet_TCP_DelSocket(theSocketSet, theClientSocket);
   SDLNet_TCP_Close(theClientSocket);
   theClientSocket = nullptr;
}

