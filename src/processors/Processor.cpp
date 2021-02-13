#include <algorithm>
#include "Processor.h"
#include "Logger.h"
#include "Debugger.h"
#include "Display.h"

Processor::Processor():
    theMemoryController(nullptr),
    thePc(0),
    theInstructionsExecuted(0)
{
	
}

Processor::~Processor()
{
   
}

CpuAddress Processor::getPc()
{
	return thePc;
}

uint64_t Processor::getInstructionsCount()
{
	return theInstructionsExecuted;
}

void Processor::setMemoryController(MemoryController* mc)
{
   theMemoryController = mc;
}

int Processor::getNumberOfBytesForState()
{
    return -1;
}

void Processor::writeState(uint8_t* stateData)
{
    LOG_DEBUG() << "Processor doesn't implement save states";
}

void Processor::loadState(uint8_t stateData)
{
    LOG_DEBUG() << "Processor doesn't implement save states";
}

DECLARE_DEBUGGER_CALLBACK(Processor, registersCommandHandler);
DECLARE_DEBUGGER_CALLBACK(Processor, stepCommandHandler);
DECLARE_DEBUGGER_CALLBACK(Processor, runCommandHandler);
DECLARE_DEBUGGER_CALLBACK(Processor, disassCommandHandler);
DECLARE_DEBUGGER_CALLBACK(Processor, resetCommandHandler);

void Processor::registerDebugHandlerCommands(Debugger* dbgr)
{
   dbgr->registerNewCommandHandler("regs", "Prints / Sets values of the registers",
                                   g_registersCommandHandler,
                                   this);
   dbgr->registerNewCommandHandler("step", "Steps CPU through 1 or more instruction",
                                   g_stepCommandHandler,
                                   this);
   dbgr->registerNewCommandHandler("run", "Execute CPU forever",
                                   g_runCommandHandler,
                                   this);
   dbgr->registerNewCommandHandler("disass", "Disassembles instructions",
                                   g_disassCommandHandler,
                                   this);
   dbgr->registerNewCommandHandler("reset", "Resets System",
                                   g_resetCommandHandler,
                                   this);
}


void Processor::registersCommandHandler(std::vector<std::string> const & args)
{
    if (args.size() == 2)
    {
       // Trying to set a register?
       std::vector<std::string> nameList = getRegisterNames();
       if(std::find(nameList.begin(), nameList.end(), args[0]) == nameList.end())
       {
          std::cout << "No register named " << args[0] << std::endl;
          return;
       }
       
       bool success = false;
       uint32_t value = Utils::parseUInt32(args[1], &success);
       if (!success)
       {
          std::cout << "Invalid value to set register: " << args[1] << std::endl;
          return;
       }
       
       setRegisterValue(args[0], value);
       return;
    }
   
    int i = 0;
    char buffer[200];
    for(auto curReg: getRegisterNames())
    {
        snprintf(buffer, 200, "%8s=0x%08x", curReg.c_str(), getRegisterValue(curReg));
        if (i % 4 == 3)
        {
            // end of the line
            std::cout << buffer << std::endl;
        }
        else
        {
            std::cout << buffer << "\t"; 
        }
        
        i++;
    }
    
    if (i % 4)
    {
        std::cout << std::endl;
    }
}


void Processor::stepCommandHandler(std::vector<std::string> const & args)
{

   Display::getInstance()->processQueues();

   int stepCount = 1;
   if (args.size() > 0)
   {
      // User specified a step count
      stepCount = atoi(args[0].c_str());
   }

   for(int i = 0; i < stepCount; i++)
   {
      if (!step())
      {
         std::cout << "Emulation halted in step function" << std::endl;
         break;
      }

      if(!Display::getInstance()->processQueues())
      {
         std::cout << "Emulation halted by SDL event" << std::endl;
         break;
      }
   }
   
   std::vector<std::string> emptyArgList;
   registersCommandHandler(emptyArgList);
   
   std::cout << "InsCount=" << theInstructionsExecuted << std::endl;

   std::string assembly;
   disassembleAddr(thePc, &assembly);
   std::cout << assembly << std::endl;


}

void Processor::runCommandHandler(std::vector<std::string> const & args)
{
   Display::getInstance()->processQueues();

   uint32_t startTimeTicks = SDL_GetTicks();
   uint64_t theRunCommandStartInsCount = theInstructionsExecuted;

   while(true)
   {
      if (!step())
      {
         std::cout << "Emulation halted in run function" << std::endl;
         break;
      }

      if(!Display::getInstance()->processQueues())
      {
         std::cout << "Emulation halted by SDL event" << std::endl;
         break;
      }
   }

   uint32_t stopTimeTicks = SDL_GetTicks();
   double numExecuted = theInstructionsExecuted - theRunCommandStartInsCount;

   std::vector<std::string> emptyArgList;
   registersCommandHandler(emptyArgList);

   double instructionsPerSec = numExecuted * 1000.0 / ( (double) (stopTimeTicks - startTimeTicks) );

   std::cout << "InsCount=" << theInstructionsExecuted << "\t";

   if (instructionsPerSec > 1000000.0)
   {
      std::cout << instructionsPerSec / 1000000.0 << " MHz" << std::endl;
   }
   else if (instructionsPerSec > 1000.0)
   {
      std::cout << instructionsPerSec / 1000.0 << " kHz" << std::endl;
   }
   else
   {
      std::cout << instructionsPerSec << " Hz" << std::endl;
   }

   std::string assembly;
   disassembleAddr(thePc, &assembly);
   std::cout << assembly << std::endl;
}

void Processor::disassCommandHandler(std::vector<std::string> const & args)
{
   // Is there a specific address the user wants to disassemble?
   CpuAddress addr = getPc();
   if (args.size() >= 1)
   {
      if (!stringToAddress(args[0], &addr))
      {
         std::cout << "Error converting param to address: " << args[0] << std::endl;
         return;
      }
   }
   
   for(int i = 0; i < 10; i++)
   {
      std::string listing;
      int numBytes = disassembleAddr(addr, &listing);
      
      if (numBytes < 0)
      {
         std::cout << "Error disassembling @ " << addressToString(addr) << std::endl;;
         return;
      }
      
      std::cout << listing << std::endl;
      addr += numBytes;
      
      if (numBytes == 0)
      {
         // End of code path
         return;
      }
   }
}

void Processor::resetCommandHandler(std::vector<std::string> const & args)
{
   resetState();
}
