#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "EmulatorConfig.h"

#include "DisplayManager.h"

// Memory devices
#include "NesRom.h"
#include "RomMemory.h"
#include "RamMemory.h"
#include "UartDevice.h"
#include "MirrorMemory.h"
#include "RngDev.h"

#include "MemoryController.h"
#include "Cpu6502.h"
#include "Logger.h"
#include "MemoryConfig.h"

#include <SDL.h>
#include <SDL_net.h>

void printUsage(char* appName)
{
   std::cout << appName << " is 6502 emulator" << std::endl;
   std::cout << appName << " -f filename -b baseaddr [-d port] [-j addr] [-h]" << std::endl;
   std::cout << "  -f --file       Filename of binary data to load" << std::endl;
   std::cout << "  -b --baseaddr   Base address of the file to disassemble" << std::endl;
   std::cout << "  -d --debugger   Enable remote debugger (port number)" << std::endl;
   std::cout << "  -j --jump       Address to jump to when emulation starts" << std::endl;
   std::cout << "  -h --help       Show this help" << std::endl;
   std::cout << "  -c --config     Configuration file" << std::endl;

#ifdef TRACE_EXECUTION
   std::cout << "  -n --numsteps   Number of instructions to execute" << std::endl;
#endif

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
#ifdef TRACE_EXECUTION
      { "numsteps", required_argument, 0, 'n'},
#endif
      { 0,          0,                 0, 0}
   };

   // Default settings
   std::string filename   = "";
   std::string configFilename = "";
   CpuAddress baseAddress = 0x0;
   bool baseAddressDefined = false;
   uint16_t debuggerPort = 0;
   bool debuggerEnabled = false;

   CpuAddress executionEntryPoint = 0;
   bool jumpAddressSet = false;

   int optIndex;

#ifdef TRACE_EXECUTION
   uint64_t numSteps = 0xffffffffffffffff;
#endif

   while(true)
   {
      char optChar = getopt_long(argc, argv, "c:f:b:j:d:h:n:", long_options, &optIndex);

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
         executionEntryPoint = addr;
         jumpAddressSet = true;
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

      case 'c':
         LOG_DEBUG() << "Config File:" << optarg;
         configFilename = optarg;
         break;

#ifdef TRACE_EXECUTION
      case 'n':
         LOG_DEBUG() << "Num Steps:" << optarg;
         numSteps = Utils::parseUInt64(optarg);
         break;
#endif

      default:
         std::cerr << "Invalid argument.  Use -h or --help to see usage" << std::endl;
      }
   }

   if (!initializeSdl())
   {
      return 1;
   }

   MemoryController* memControl = new MemoryController();
   RomMemory* optionalRomMemory = nullptr;

   if ( (filename != "") && baseAddressDefined)
   {
      // The user has specified they want to load a file directly into memory
      optionalRomMemory = new RomMemory("User_CLI_Optional");
      optionalRomMemory->setIntConfigValue("startAddress", baseAddress);
      optionalRomMemory->setStringConfigValue("romFilename", filename);

      if (optionalRomMemory->isFullyConfigured())
      {
         LOG_DEBUG() << "Adding optional CLI ROM memory to the memory controller";
         optionalRomMemory->resetMemory();
         memControl->addNewDevice(optionalRomMemory);
      }
      else
      {
         LOG_WARNING() << "Error adding the optional CLI ROM memory";
      }
   }

   std::string displayType;

   if (configFilename.length() != 0)
   {
      MemoryConfig memConfig(memControl);

      memConfig.registerMemoryDevice(RamMemory::getTypeName(),    RamMemory::getMDC());
      memConfig.registerMemoryDevice(RomMemory::getTypeName(),    RomMemory::getMDC());
      memConfig.registerMemoryDevice(UartDevice::getTypeName(),   UartDevice::getMDC());
      memConfig.registerMemoryDevice(NesRom::getTypeName(),       NesRom::getMDC());
      memConfig.registerMemoryDevice(MirrorMemory::getTypeName(), MirrorMemory::getMDC());
      memConfig.registerMemoryDevice(RngDev::getTypeName(),       RngDev::getMDC());

      std::string errorStr;
      std::string configData = Utils::loadFile(configFilename, errorStr);

      if (errorStr.length() != 0)
      {
         LOG_WARNING() << "Error loading file " << configFilename << ": " << errorStr;
      }
      else
      {
         // LOG_DEBUG() << "Here lies the config data\n" << configData;
         memConfig.parseConfig(configData);
      }

      if (memConfig.isStartAddressSet())
      {
         executionEntryPoint = memConfig.getStartAddress();
         LOG_DEBUG() << "Set execution entry point to " << executionEntryPoint << " from config";
      }

      displayType = memConfig.getDisplayType();
   }

   constructCpuGlobals();

   Cpu6502* emu = new Cpu6502(memControl);

   DisplayManager* dispManager = DisplayManager::getInstance();
   dispManager->configureDisplay(displayType, emu);
   dispManager->setMemoryController(memControl);

   if (debuggerEnabled)
   {
      if (emu->enableDebugger(debuggerPort))
      {
         LOG_DEBUG() << "Debugger started successfully on port" << debuggerPort;
      }
      else
      {
         goto FAILED_TO_START;
      }
   }

#ifdef TRACE_EXECUTION
   emu->setStepLimit(numSteps);
#endif

   if (jumpAddressSet)
   {
      executionEntryPoint = baseAddress;
      LOG_DEBUG() << "Set execution entry point to " << executionEntryPoint << " from CLI options";
   }

   // Start of emulation
   emu->setAddress(executionEntryPoint);

   // Display manager will spawn emulator thread and block for the entire duration of emulation
   dispManager->startEmulator();

   FAILED_TO_START:

   // Emulation complete, cleanup
   DisplayManager::destroyInstance();

   delete emu;

   delete memControl;

   shutdownSdl();

   return 0;
}
