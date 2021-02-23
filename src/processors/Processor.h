#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>
#include <cstdint>

#include "EmulatorCommon.h"
#include "MemoryController.h"

class Debugger;

/**
 * Processor
 */
class Processor
{
public:
   Processor();
   
   virtual ~Processor();
   
   virtual std::vector<std::string> getRegisterNames() = 0;
   
   virtual uint32_t getRegisterValue(std::string regName) = 0;
   
   virtual bool setRegisterValue(std::string regName, uint32_t value) = 0;
   
   virtual uint32_t getInstructionLength(CpuAddress addr) = 0;
   
   virtual std::string getCpuName() = 0;
   
   CpuAddress getPc();
   
   uint64_t getInstructionsCount();
   
   virtual void setMemoryController(MemoryController* mc);
   
   virtual bool step() = 0;
   
   /// Returns -1 if it doesn't supprot serializing the state
   virtual int getSaveStateLength();
   
   virtual bool saveState(uint8_t* buffer, uint32_t* bytesSaved);
   
   virtual bool loadState(uint8_t* buffer, uint32_t* bytesLoaded);
   
   virtual void resetState() = 0;
   
   // Return the number of byte disassembled, or negative on fault, 0 on end of code path
   virtual int disassembleAddr(CpuAddress addr, std::string* listingDataRet) = 0;
   
   virtual void registerDebugHandlerCommands(Debugger* dbgr);
   
   void registersCommandHandler(std::vector<std::string> const & args);
   
   void stepCommandHandler(std::vector<std::string> const & args);

   void runCommandHandler(std::vector<std::string> const & args);
   
   void disassCommandHandler(std::vector<std::string> const & args);
   
   void resetCommandHandler(std::vector<std::string> const & args);

   void saveCommandHandler(std::vector<std::string> const & args);
   void loadCommandHandler(std::vector<std::string> const & args);


protected:
   
   MemoryController* theMemoryController;
   
   CpuAddress thePc;
   
   uint64_t theInstructionsExecuted;  
};

typedef Processor* (*ProcessorConstructor)(std::string instanceName);

#endif

