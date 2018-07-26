#include <iostream>

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<getopt.h>
#include "RomMemory.h"
#include "MemoryController.h"
#include "Cpu6502.h"
#include "Logger.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

void printUsage(char* appName)
{
   std::cout << appName << " is 6502 emulator" << std::endl;
   std::cout << appName << " -f filename -b baseaddr [-d port] [-j addr] [-h]" << std::endl;
   std::cout << "  -f --file       Filename of the file to disassemble" << std::endl;
   std::cout << "  -b --baseaddr   Base address of the file to disassemble" << std::endl;
   std::cout << "  -d --debugger   Enable remote debugger (port number)" << std::endl;
   std::cout << "  -j --jump       Address to jump to when emulation starts" << std::endl;
   std::cout << "  -h --help       Show this help" << std::endl;
   std::cout << std::endl;
}

bool initializeSdl()
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
   {
      LOG_FATAL() << "SDL Initialization Failed";
      return false;
   }

   if (SDLNet_Init() != 0)
   {
      LOG_FATAL() << "SDL Networking Initialization Failed";
      return false;
   }

   LOG_DEBUG() << "SDL Initialized";
   return true;
}

void shutdownSdl()
{
   SDLNet_Quit();
   LOG_DEBUG() << "SDL Net Shutdown";

   SDL_Quit();
   LOG_DEBUG() << "SDL Shutdown";
}

int main(int argc, char* argv[])
{
   struct option long_options[] = {
      { "file",     required_argument, 0, 'f'},
      { "baseaddr", required_argument, 0, 'b'},
      { "jump",     required_argument, 0, 'j'},
      { "debugger", required_argument, 0, 'd'},
      { "help",     no_argument,       0, 'h'},
      { 0,          0,                 0, 0}
   };

   // Default settings
   std::string filename   = "";
   CpuAddress baseAddress = 0x0;
   bool baseAddressDefined = false;
   uint16_t debuggerPort = 0;
   bool debuggerEnabled = false;

   std::vector<CpuAddress> executionEntryPoints;

   int optIndex;

   while(true)
   {
      char optChar = getopt_long(argc, argv, "f:b:j:d:h", long_options, &optIndex);

      if (optChar == -1)
      {
         //qDebug() << "Arg parsing complete";
         break;
      }

      switch (optChar)
      {
      case 'f':
         LOG_DEBUG() << "Filename:" << optarg;
         filename = optarg;
         break;

      case 'b':
      {
         baseAddress = Utils::parseUInt16(optarg);
         LOG_DEBUG() << "Base Address:" << addressToString(baseAddress);
         baseAddressDefined = true;
         break;
      }

      case 'j':
      {
         CpuAddress addr = Utils::parseUInt16(optarg);
         LOG_DEBUG() << "Execution Entry Point:" << addressToString(addr);
         executionEntryPoints.push_back(addr);
         break;
      }

      case 'd':
      {
         debuggerPort = Utils::parseUInt16(optarg);
         debuggerEnabled = true;
         LOG_DEBUG() << "Debugger enabled on port:" << debuggerPort;
         break;
      }

      case 'h':
      {
         printUsage(argv[0]);
         return 0;
      }

      default:
         std::cerr << "Invalid argument.  Use -h or --help to see usage" << std::endl;
      }
   }

   if ( (filename == "") || (!baseAddressDefined) )
   {
      LOG_WARNING() << "Filename and base address are required parameters!";
      printUsage(argv[0]);
      return 1;
   }

   if (!initializeSdl())
   {
      return 1;
   }

   LOG_DEBUG() << "Loading " << filename << " at address " << addressToString(baseAddress);

   RomMemory programData(filename, baseAddress);

   MemoryController memControl;
   memControl.addNewDevice( (MemoryDev*) &programData);
   {
      Cpu6502 emu(&memControl);

      if (debuggerEnabled)
         emu.enableDebugger(debuggerPort);


      SDL_Delay(100000);
   }

   shutdownSdl();

   return 0;
}
