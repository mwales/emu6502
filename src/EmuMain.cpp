#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "EmulatorConfig.h"
#include "DisplayManager.h"
#include "ConfigManager.h"
#include "MemoryFactory.h"
#include "MemoryController.h"
#include "Cpu6502.h"
#include "Logger.h"

#include <SDL.h>
#include <SDL_net.h>

const std::string DEBUGGER_TYPE = "debugger";
const std::string DEBUGGER_PORT = "port";
const std::string CONFIGFILE_TYPE = "config";
const std::string CONFIGFILE_NAME = "filename";
const std::string EMULATOR_TYPE = "emulator";
const std::string EMUSTART_NAME = "startAddress";
const std::string TRACESTEPS_NAME = "stepCount";

void printUsage(char* appName)
{
   std::cout << appName << " is 6502 emulator" << std::endl;

   std::cout << std::endl;

   std::cout << "  " << CONFIGFILE_TYPE << ".uniquename." << CONFIGFILE_NAME << "=config.txt" << std::endl;
   std::cout << std::endl;

   std::cout << "  ROM.uniquename.filename=data.bin" << std::endl;
   std::cout << "  ROM.uniquename.address=0x1234" << std::endl;
   std::cout << "  ROM.uniquename.startEmulatorAddress=0x1234   (optional)" << std::endl;
   std::cout << std::endl;

   std::cout << "  RAM.uniquename.size=0x1000" << std::endl;
   std::cout << "  RAM.uniquename.startAddress=0x0000" << std::endl;
   std::cout << std::endl;

   std::cout << "  RNG.uniquename.size=1" << std::endl;
   std::cout << "  RNG.uniquename.startAddress=0xff" << std::endl;
   std::cout << std::endl;

   std::cout << "  MirrorMemory.uniquename.size=0x100" << std::endl;
   std::cout << "  MirrorMemory.uniquename.cloneDest=0x100" << std::endl;
   std::cout << "  MirrorMemory.uniquename.cloneSource=0x200" << std::endl;
   std::cout << std::endl;

   std::cout << "  " << DEBUGGER_TYPE << ".uniquename." << DEBUGGER_PORT << "=6502" << std::endl;
   std::cout << std::endl;

   std::cout << "  iNES.uniquename.filename=game.nes" << std::endl;
   std::cout << std::endl;

   std::cout << "  Easy6502InputDevice.uniquename.dontcare=dontcare" << std::endl;
   std::cout << std::endl;

   std::cout << "  Easy6502JsDisplay.uniquename.dontcare=dontcare" << std::endl;
   std::cout << std::endl;

   std::cout << "  " << EMULATOR_TYPE << ".system." << EMUSTART_NAME << "=0x1234" << std::endl;


#ifdef TRACE_EXECUTION
   std::cout << "  " << EMULATOR_TYPE << ".system." << TRACESTEPS_NAME << "=100" << std::endl;
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
   if (argc == 1)
   {
      printUsage(argv[0]);
      return 0;
   }

   ConfigManager* configMgr = ConfigManager::createInstance();
   configMgr->processArgs(argc, argv);

   // Did the user specify a config file
   if(configMgr->isConfigPresent(CONFIGFILE_TYPE, CONFIGFILE_NAME))
   {
      std::string configFileName = configMgr->getStringConfigValue(CONFIGFILE_TYPE, CONFIGFILE_NAME);

      LOG_DEBUG() << "Load configuration from file: " << configFileName;
      configMgr->loadConfigFile(configFileName);
   }
   else
   {
      LOG_DEBUG() << "No config file specified";
   }

   // Default settings
   std::string filename   = "";
   std::string configFilename = "";
   uint16_t debuggerPort = 0;
   bool debuggerEnabled = false;

#ifdef TRACE_EXECUTION
   uint64_t numSteps = 0xffffffffffffffff;
#endif

   if (!initializeSdl())
   {
      return 1;
   }

   MemoryController* memControl = new MemoryController();

   MemoryFactory memFactory(memControl);
   memFactory.processConfigData();

   // Is the debugger configured?
   if (configMgr->isConfigPresent(DEBUGGER_TYPE, DEBUGGER_PORT))
   {
      debuggerPort = configMgr->getIntegerConfigValue(DEBUGGER_TYPE, DEBUGGER_PORT);
      debuggerEnabled = true;
   }

   CpuAddress startAddress = 0;
   if (configMgr->isConfigPresent(EMULATOR_TYPE, EMUSTART_NAME))
   {
      startAddress = configMgr->getIntegerConfigValue(EMULATOR_TYPE, EMUSTART_NAME);
      LOG_DEBUG() << "Start Address set by CLI:" << addressToString(startAddress);

      // todo do something with the start address
   } else if (memControl->getStartAddress(&startAddress))
   {
      LOG_DEBUG() << "Start Address set my memory controller:" << addressToString(startAddress);
   }
   else
   {
      LOG_WARNING() << "No start address found!";
   }

   constructCpuGlobals();

   Cpu6502* emu = new Cpu6502(memControl);

   emu->setAddress(startAddress);

   DisplayManager* dispManager = DisplayManager::getInstance();
   dispManager->setMemoryController(memControl);
   dispManager->configureDisplay(emu);

   memControl->resetAll();

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
   if (configMgr->isConfigPresent(EMULATOR_TYPE, TRACESTEPS_NAME))
   {
      numSteps = configMgr->getIntegerConfigValue(EMULATOR_TYPE, TRACESTEPS_NAME);
      LOG_DEBUG() << "Trace Exectuion limited to" << numSteps << "steps";
   }
   emu->setStepLimit(numSteps);
#endif

   // Display manager will spawn emulator thread and block for the entire duration of emulation
   dispManager->startEmulator();

   FAILED_TO_START:

   // Emulation complete, cleanup
   DisplayManager::destroyInstance();

   delete emu;

   delete memControl;

   shutdownSdl();

   configMgr->destroyInstance();

   return 0;
}
