#include "Disassembler6502.h"
#include "Logger.h"



Disassembler6502::Disassembler6502(MemoryController* memCtrl)
{
   theMemoryController = memCtrl;
}

void Disassembler6502::start(CpuAddress address)
{
   decode(address);
}

void Disassembler6502::halt()
{
   theHaltFlag = true;
}

void Disassembler6502::printDisassembly()
{
   LOG_DEBUG() << __PRETTY_FUNCTION__;
}









void Disassembler6502::load(CpuAddress instAddr, uint8_t opCodes)
{

}

void Disassembler6502::store(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::transfer(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::add(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::subtract(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::increment(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::decrement(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::andOperation(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::orOperation(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::xorOperation(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::shiftLeft(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::shiftRight(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::rotateLeft(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::rotateRight(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::push(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::pull(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::jump(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::jumpSubroutine(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::returnFromInterrupt(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::returnFromSubroutine(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::bitTest(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::compare(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::clear(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::set(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::noOp(CpuAddress instAddr, uint8_t opCodes)
{

}
void Disassembler6502::breakOperation(CpuAddress instAddr, uint8_t opCodes)
{
   LOG_DEBUG() << "Break op";
}
void Disassembler6502::branch(CpuAddress instAddr, uint8_t opCodes)
{

}
