#include "Processor.h"
#include "Logger.h"

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

virtual int Processor::getNumberOfBytesForState()
{
    return -1;
}

virtual void Processor::writeState(uint8_t* stateData)
{
    LOG_DEBUG() << "Processor doesn't implement save states";
}

virtual void Processor::loadState(uint8_t stateData)
{
    LOG_DEBUG() << "Processor doesn't implement save states";
}

