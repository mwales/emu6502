#include "Disassembler.h"
#include "Logger.h"



Disassembler::Disassembler(MemoryController* memCtrl)
{
   theMemoryController = memCtrl;
}

void Disassembler::start(CpuAddress address)
{

}

void Disassembler::halt()
{
   theHaltFlag = true;
}

void Disassembler::printDisassembly()
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}
