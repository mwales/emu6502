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
   
   void setDisassemblyOptions(bool includeOpCodes, bool includeAddress);
   
   bool step();
	
protected:
	
	MemoryController* theMemoryController;
	
	CpuAddress thePc;
	
	uint64_t theInstructionsExecuted;
   
   bool theDisassFlagIncludeOpCodes;
   
   bool theDisassFlagIncludeAddress;
};

typedef Processor* (*ProcessorConstructor)(std::string instanceName);

#endif
