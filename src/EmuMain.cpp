#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "EmulatorConfig.h"
#include "Display.h"
#include "ConfigManager.h"
#include "MemoryFactory.h"
#include "MemoryController.h"
#include "SimpleMap.h"
#include "Debugger.h"
#include "ProcessorFactory.h"
#include "Processor.h"

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
   std::cout << appName << " is generic debugger / emulator" << std::endl;
   std::cout << std::endl;

   std::cout << "Configuration can be provided to the emulator via command line args or through" << std::endl;
   std::cout << "Coniguration variable format is type.instance_name.var_name=value" << std::endl;
   std::cout << std::endl;
   std::cout << "Configuration Type: config" << std::endl;
   std::cout << "    String config: filename" << std::endl;
   std::cout << std::endl;
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
      MemoryFactory::getInstance()->printMemoryConfigHelp();
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

   if (!initializeSdl())
   {
      return 1;
   }

   MemoryController* memControl = new MemoryController();

   MemoryFactory::getInstance()->instantiateMemoryDevices(memControl);
   
   Processor* cpu = ProcessorFactory::getInstance()->instantiateProcessor();

   Debugger d(memControl);
   
   memControl->registerDebuggerCommands(&d);
   
   if (cpu)
   {
      cpu->setMemoryController(memControl);
      cpu->registerDebugHandlerCommands(&d);
   }

   Display* disp = Display::createInstance();
   disp->registerDebuggerCommands(&d);
   disp->processQueues();

   cpu->resetState();
   
   d.start();

   LOG_DEBUG() << "Shutting down emulator";

   disp->destroyInstance();
   disp = nullptr;


//   constructCpuGlobals();

//   Cpu6502* emu = new Cpu6502(memControl);

//   emu->setAddress(startAddress);

//   DisplayManager* dispManager = DisplayManager::getInstance();
//   dispManager->setMemoryController(memControl);
//   dispManager->configureDisplay(emu);

//   memControl->resetAll();

//   if (debuggerEnabled)
//   {
//      if (emu->enableDebugger(debuggerPort))
//      {
//         LOG_DEBUG() << "Debugger started successfully on port" << debuggerPort;
//      }
//      else
//      {
//         goto FAILED_TO_START;
//      }
//   }

//#ifdef TRACE_EXECUTION
//   if (configMgr->isConfigPresent(EMULATOR_TYPE, TRACESTEPS_NAME))
//   {
//      numSteps = configMgr->getIntegerConfigValue(EMULATOR_TYPE, TRACESTEPS_NAME);
//      LOG_DEBUG() << "Trace Exectuion limited to" << numSteps << "steps";
//   }
//   emu->setStepLimit(numSteps);
//#endif

//   // Display manager will spawn emulator thread and block for the entire duration of emulation
//   dispManager->startEmulator();

//   FAILED_TO_START:

//   // Emulation complete, cleanup
//   DisplayManager::destroyInstance();
   
   if(cpu)
   {
      delete cpu;
      cpu = nullptr;
   }

   delete memControl;

   shutdownSdl();

   configMgr->destroyInstance();

   return 0;
}
