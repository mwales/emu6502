#include "Processor.h"
#include "Logger.h"

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

void Processor::printRegistersCommandHandlerStatic(std::vector<std::string> const & args, 
                                          void* context)
{
    Processor* p = reinterpret_cast<Processor*>(context);
    p->printRegistersCommandHandler(args);
}

void Processor::printRegistersCommandHandler(std::vector<std::string> const & args)
{
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


