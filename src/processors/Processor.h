#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>
#include <cstdint>

#include "EmulatorCommon.h"
#include "MemoryController.h"


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
   
   virtual std::string getDisassembly(CpuAddress addr) = 0;
   
   virtual std::string getCpuName() = 0;
   
   CpuAddress getPc();
   
   uint64_t getInstructionsCount();
   
   virtual void setMemoryController(MemoryController* mc);
   
   virtual void setDisassemblyOptions(bool includeOpCodes, bool includeAddress);
   
   virtual bool step() = 0;
   
   /// Returns -1 if it doesn't supprot serializing the state
   virtual int getNumberOfBytesForState();
   
   virtual void writeState(uint8_t* stateData);
   
   virtual void loadState(uint8_t stateData);
   
   virtual void resetState() = 0;
   
protected:
   
   MemoryController* theMemoryController;
   
   CpuAddress thePc;
   
   uint64_t theInstructionsExecuted;
   
   bool theDisassFlagIncludeOpCodes;
   
   bool theDisassFlagIncludeAddress;
};

typedef Processor* (*ProcessorConstructor)(std::string instanceName);

#endif

