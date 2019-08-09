#include "DebugServer.h"
#include "EmulatorConfig.h"

#include <cstring>

#include "Logger.h"
#include "Cpu6502.h"
#include "Cpu6502Defines.h"
#include "MemoryController.h"
#include "MemoryDev.h"
#include "Disassembler6502.h"

#ifdef DEBUG_SERVER_DEBUG
   #define DS_DEBUG   LOG_DEBUG
   #define DS_WARNING LOG_WARNING
#else
   #define DS_DEBUG   if(0) LOG_DEBUG
   #define DS_WARNING if(0) LOG_WARNING
#endif

DebugServer::DebugServer(Cpu6502* cpu, uint16_t portNum, MemoryController* memController):
   theCpu(cpu),
   theMemoryController(memController),
   thePortNumber(portNum),
   theServerSocket(nullptr),
   theClientSocket(nullptr),
   theQuitFlag(false),
   theNumberBytesToRx(-1),
   theRegisterDumpSentToClient(false),
   thePausedOnBreakpointFlag(false)
{
   DS_DEBUG() << "DebugServer constructed, portNum = " << portNum;

   theSocketSet = SDLNet_AllocSocketSet(4);
}

DebugServer::~DebugServer()
{
   DS_DEBUG() << "DebugServer destructor called";

   SDLNet_TCP_Close(theServerSocket);
   SDLNet_FreeSocketSet(theSocketSet);
}

bool DebugServer::startDebugServer()
{
   // Create socket
   IPaddress ip;
   if (SDLNet_ResolveHost(&ip, NULL, thePortNumber) != 0)
   {
      DS_WARNING() << "ResolveHost call failed:" << SDLNet_GetError();
      return false;
   }

   theServerSocket = SDLNet_TCP_Open(&ip);
   if (!theServerSocket)
   {
      DS_WARNING() << "TCP Open call failed:" << SDLNet_GetError();
      return false;
   }

   return true;
}

int DebugServer::debugHook()
{
   // Check breakpoints
   int retVal = 0;
   CpuAddress curAddr = theCpu->getPc();
   if (theBreakpoints.find(curAddr) != theBreakpoints.end())
   {
      // We hit the breakpoint
      if (thePausedOnBreakpointFlag)
      {
         // We already hit this breakpoint and stopped the debugger.  We don't
         // need the breakpoint to change debugger state anymore until next
         // breakpoint hit
         thePausedOnBreakpointFlag = false;
      }
      else
      {
         thePausedOnBreakpointFlag = true;
         theDebuggerState.pauseEmulator();
         DS_DEBUG() << "Debugger hit breakpoint at" << addressToString(curAddr);
      }
   }

   if (theQuitFlag)
   {
      DS_DEBUG() << "debugHook letting caller know client has called quit";
      return -1;
   }

   if (!theDebuggerState.emulatorAllowExecution())
   {
      // DS_DEBUG() << "Emulator not executing";
      SDL_Delay(5);
      retVal = 0;
   }
   else
   {
      retVal = 1;
   }

   handleDebuggerClients();

   return retVal;
}

void DebugServer::debugMemoryAccessHook(CpuAddress addr, bool isWrite)
{
   if (theMemoryAccessBPs.find(addr) != theMemoryAccessBPs.end() )
   {
      theDebuggerState.pauseEmulator();
      DS_DEBUG() << "Debugger hit memory access breakpoint for "
                 << (isWrite ? "write" : "read") << " at "
                 << addressToString(addr);
   }
   else
   {
      DS_DEBUG() << "Hook called for " << (isWrite ? "write" : "read") << "memory access at "
                 << addressToString(addr);
   }
}

void DebugServer::emulatorHalt(void* thisPtr)
{
   DS_DEBUG() << "DebugServer::emulatorHalt called";

   DebugServer* ds = (DebugServer*) thisPtr;

   ds->theDebuggerState.pauseEmulator();
}

int DebugServer::handleDebuggerClients()
{
   DS_DEBUG() << "DebuggerServer::handleDebuggerClients";

   TCPsocket newConnection;
   newConnection = SDLNet_TCP_Accept(theServerSocket);

   if (newConnection)
   {
      // We just got a new connection!
      DS_DEBUG() << "Accepted a new debugger connection";

      theNumberBytesToRx = -1;
      memset(theRxDataBuffer, 0, DEBUGGER_MAX_MSG_LEN);

      if (theClientSocket)
         closeExistingConnection("New connection");

      theClientSocket = newConnection;
      SDLNet_TCP_AddSocket(theSocketSet, theClientSocket);
   }

   // Any activities on the sockets?
   int activity = SDLNet_CheckSockets(theSocketSet, 5);

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
      }

      uint16_t numBytesFrame = SDLNet_Read16(theRxDataBuffer);
      uint16_t command = SDLNet_Read16(theRxDataBuffer + 2);

      DS_DEBUG() << "Received a header.  Frame Size = " << numBytesFrame
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
            DS_WARNING() << "Error.  Only received " << bytesReceivedTotal
                         << "bytes of frame with " << numBytesFrame << " bytes";
            closeExistingConnection("Truncated frame");
            break;
         }

         bytesReceivedTotal += bytesReceived;
      }

      DS_DEBUG() << "Received full frame of " << numBytesFrame << "bytes, cmd = "
                 << command;

      processCommand(numBytesFrame, command);
   }

   // Check for fresh halts from the emulator
   if (theDebuggerState.isFreshHalt())
   {
      // the take was successsful, the emulator just halted
      if (theClientSocket)
      {
         DS_DEBUG() << "Emulator halted, sending register dump to debugger client";
         dumpRegistersCommand();
         theDebuggerState.acknowledgeHalt();
      }
      else
      {
         DS_DEBUG() << "Emulator halted, no debugger client to alert";
      }
   }

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
      DS_WARNING() << "Failed to send response.  " << bytesSent << " bytes send of message length "
                   << (msgLen + 2);
      closeExistingConnection("Failed to send message to the client");
   }
   else
   {
      DS_DEBUG() << "Sent a " << bytesSent << " byte message to the debugger client";
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

   case 3: // LIST
      disassembleCommand(commandLen);
      break;

   case 4: // dump registers
      theRegisterDumpSentToClient = false;
      dumpRegistersCommand();
      break;

   case 5: // STEP
      stepCommand(commandLen);
      break;

   case 6: // HALT
      haltCommand();
      break;

   case 7: // CONTINUE
      continueCommand();
      break;

   case 8: // Memory Dump
      memoryDumpCommand(commandLen);
      break;

   case 9: // Breakpoint add
   case 12: // Add memory access breakpoint
      addBreakpointCommand(command, commandLen);
      break;

   case 10: // Breakpoint remove
   case 13: // Delete memory access breakpoint
      removeBreakpointCommand(command, commandLen);
      break;

   case 11: // Breakpoint list
      listBreakpointCommand();
      break;

   // case 12 is above
   // case 13 is above

   default:
      DS_WARNING() << "Command " << command << " is not implemented!";
   }
}

void DebugServer::versionCommand()
{
   DS_DEBUG() << "Version command sent by the debugger client";

   char const * versionString = "6502 Emulator Version 0.0 (No System)";
   sendResponse(strlen(versionString), (uint8_t*) versionString);
}

void DebugServer::quitCommand()
{
   DS_DEBUG() << "Exit emulator command sent by debugger client";

   // Stop the emulator
   theDebuggerState.pauseEmulator();

   theQuitFlag = true;
}

void DebugServer::disassembleCommand(uint16_t commandLen)
{
   // Command body should have address for listing, and number instructions to return
   int expectedLen = sizeof(uint8_t) + sizeof(CpuAddress) + sizeof(uint16_t);

   if (commandLen != expectedLen)
   {
      // They didn't a command of the expected length
      char const * malformedDisassCommandString = "List command data is malformed";
      sendResponse(strlen(malformedDisassCommandString), (uint8_t*) malformedDisassCommandString);
      return;
   }

   // Pull the address from the command data
   uint8_t flags = theRxDataBuffer[0];
   CpuAddress addr = (CpuAddress) SDLNet_Read16(theRxDataBuffer + 1);
   uint16_t numInstructionsCommand = SDLNet_Read16(theRxDataBuffer + 3);

   // DS_DEBUG() << "Disassemble command sent by debugger client.  Address = " << Utils::toHex16(addr)
   //            << ", num instructions = " << numInstructionsCommand;
   // DS_DEBUG() << "FLAGS: " << (flags & 0x1 ? "ADDRESS_PROVIDED" : "NO_ADDRESS_PROVIDED")
   //            << "   " << (flags & 0x2 ? "NUM_INS_PROVIDED" : "NO_NUM_INS_PROVIDED");

   // The default number of instructions is 5, unless user has ever specified how many
   static int numIns = 5;
   if (flags & 2)
      numIns = numInstructionsCommand;

   CpuAddress disassAddr = theCpu->getPc();
   if (flags & 1)
      disassAddr = addr;

   Disassembler6502 dis(theMemoryController);
   dis.includeOpCodes(true);
   std::string listing = dis.debugListing(disassAddr, numIns);

   sendResponse(listing.length(), (uint8_t*) listing.c_str());
}

void DebugServer::dumpRegistersCommand()
{
   DS_DEBUG() << "Dump registers command (or internally generated)";

   if (theRegisterDumpSentToClient)
   {
      DS_DEBUG() << "Dump already sent to client, not double dumping, nobody likes #2";
      return;
   }

   theRegisterDumpSentToClient = true;

   // Going to return X, Y, Accum, StackPointer, PC, StatusReg, Padding, NumClocksHigh, NumClocksLow
   const int MSG_LEN = 8 + 8;

   uint8_t msgBuf[MSG_LEN];

   uint8_t regX, regY, accum;
   theCpu->getRegisters(&regX, &regY, &accum);

   msgBuf[0] = regX;
   msgBuf[1] = regY;
   msgBuf[2] = accum;
   msgBuf[3] = theCpu->getStackPointer();

   CpuAddress pc = theCpu->getPc();

   SDLNet_Write16(pc, msgBuf + 4);

   msgBuf[6] = theCpu->getStatusReg();
   msgBuf[7] = 0;

   uint64_t numClocks = theCpu->getInstructionCount();
   uint32_t highClocks = (numClocks >> 32);
   uint32_t lowClocks = (numClocks & 0xfffffff);

   SDLNet_Write32(highClocks, msgBuf + 8);
   SDLNet_Write32(lowClocks, msgBuf + 12);

   sendResponse(16, msgBuf);
}

void DebugServer::stepCommand(uint16_t commandLen)
{
   /// We are expecting a register dump at the end of this command
   theRegisterDumpSentToClient = false;

   int stepCount = 1;
   if (commandLen != 2)
   {
      DS_WARNING() << "Debugger client didn't give us step size, we assume 1.  Fix client";
   }
   else
   {
      stepCount = SDLNet_Read16(theRxDataBuffer);
   }

   DS_DEBUG() << "Step command received from debugger client, step count = " << stepCount;
   theDebuggerState.stepEmulator(stepCount);
}

void DebugServer::continueCommand()
{
   /// We are sorta expecting a register dump at the end of this command
   theRegisterDumpSentToClient = false;

   DS_DEBUG() << "Continue command received from debugger client";
   theDebuggerState.runEmulator();
}

void DebugServer::haltCommand()
{
   /// We are expecting a register dump at the end of this command
   theRegisterDumpSentToClient = false;

   DS_DEBUG() << "Halt command received from debugger client";
   theDebuggerState.pauseEmulator();
}

void DebugServer::memoryDumpCommand(uint16_t commandLen)
{
   // We are expecting CpuAddress addr, and uint16_t len
   if (commandLen != 4)
   {
      char const * mdErrorMessage = "Malformed memory dump command received from debugger client";
      DS_WARNING() << mdErrorMessage;
      sendResponse(strlen(mdErrorMessage), (uint8_t*) mdErrorMessage);
      return;
   }

   CpuAddress addr = SDLNet_Read16(theRxDataBuffer);
   uint16_t mdSize = SDLNet_Read16(theRxDataBuffer + 2);

   DS_DEBUG() << "MemoryDump(" << Utils::toHex16(addr) << ", " << Utils::toHex16(mdSize)
              << ") received";

   uint8_t* rspBuf = (uint8_t*) malloc(mdSize + 4);
   memset(rspBuf, 0, mdSize + 4);


   for(int i = 0; i < mdSize; i++)
   {
      MemoryDev* memoryDevice = theMemoryController->getDevice(addr + i);
      if (memoryDevice != nullptr)
      {
         rspBuf[i + 4] = theMemoryController->getDevice(addr + i)->read8(addr + i);
      }
      else
      {
         DS_WARNING() << "Memory dump tried to dump invalid memory at address "
                      << Utils::toHex16(addr + i);

         // We will truncate the buffer we resond with to the valid size
         mdSize = i;
         break;
      }
   }

   SDLNet_Write16(addr, rspBuf);
   SDLNet_Write16(mdSize, rspBuf + 2);

   sendResponse(mdSize + 4, rspBuf);
}

void DebugServer::addBreakpointCommand(uint16_t command, uint16_t commandLen)
{
   // We are expecting CpuAddress addr
   if (commandLen != 2)
   {
      char const * mdErrorMessage = "Malformed add breakpoint received from debugger client";
      DS_WARNING() << mdErrorMessage;
      sendResponse(strlen(mdErrorMessage), (uint8_t*) mdErrorMessage);
      return;
   }

   CpuAddress addr = SDLNet_Read16(theRxDataBuffer);

   if (command == 9)
      theBreakpoints.insert(addr);
   else
      theMemoryAccessBPs.insert(addr);

   DS_DEBUG() << "Inserted" << addressToString(addr) << "into breakpoint list " <<
                 (command == 9 ? "instruction" : "memory access");

   sendBreakpointList();
}

void DebugServer::removeBreakpointCommand(uint16_t command, uint16_t commandLen)
{
   // We are expecting CpuAddress addr
   if (commandLen != 2)
   {
      char const * mdErrorMessage = "Malformed remove breakpoint received from debugger client";
      DS_WARNING() << mdErrorMessage;
      sendResponse(strlen(mdErrorMessage), (uint8_t*) mdErrorMessage);
      return;
   }

   CpuAddress addr = SDLNet_Read16(theRxDataBuffer);

   std::set<CpuAddress>* bpList;
   if (command == 10)
      bpList = &theBreakpoints;
   else
      bpList = &theMemoryAccessBPs;

   auto bpIt = bpList->find(addr);
   if (bpIt != bpList->end())
   {
      // We found the breakpoint in the list, now remove it
      bpList->erase(bpIt);

      DS_DEBUG() << "Removed breakpoint" << addressToString(addr);
   }
   else
   {
      DS_WARNING() << "Failed to remove breakpoint" << addressToString(addr) << ", because it is not set";
   }

   sendBreakpointList();
}

void DebugServer::listBreakpointCommand()
{
   DS_DEBUG() << "List breakpoint command received";
   sendBreakpointList();
}

void DebugServer::sendBreakpointList()
{
   uint16_t numberBreakpoints = theBreakpoints.size() + theMemoryAccessBPs.size();
   int frameLength = 2 * sizeof(uint16_t) + numberBreakpoints * sizeof(CpuAddress);

   uint8_t* frameBuffer = (uint8_t*) malloc(frameLength);
   uint8_t* fbp = frameBuffer;

   SDLNet_Write16(theBreakpoints.size(), fbp);
   fbp += sizeof(uint16_t);

   SDLNet_Write16(theMemoryAccessBPs.size(), fbp);
   fbp += sizeof(uint16_t);

   for(auto BpIt = theBreakpoints.begin(); BpIt != theBreakpoints.end(); BpIt++)
   {
      SDLNet_Write16(*BpIt, fbp);
      fbp += sizeof(CpuAddress);
   }

   for(auto BpMemIt = theMemoryAccessBPs.begin(); BpMemIt != theMemoryAccessBPs.end(); BpMemIt++)
   {
      SDLNet_Write16(*BpMemIt, fbp);
      fbp += sizeof(CpuAddress);
   }

   sendResponse(frameLength, frameBuffer);
}

void DebugServer::closeExistingConnection(char const * reason)
{
   DS_WARNING() << "Closing existing debugger client connection: " << reason;
   SDLNet_TCP_DelSocket(theSocketSet, theClientSocket);
   SDLNet_TCP_Close(theClientSocket);
   theClientSocket = nullptr;
}

