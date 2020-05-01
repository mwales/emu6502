#include "Processor.h"

Processor::Processor():
    theMemoryController(nullptr),
    thePc(0),
    theInstructionsExecuted(0),
    theDisassFlagIncludeOpCodes(true),
    theDisassFlagIncludeAddress(true)
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
