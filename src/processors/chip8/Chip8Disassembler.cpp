#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <iomanip>
#
#include <stdio.h>
#include "Chip8Disassembler.h"
#include "Utils.h"
#include "MemoryController.h"
#include "MemoryDev.h"
using namespace std;

Chip8Disassembler::Chip8Disassembler(MemoryController* mc):
    theMemoryController(mc)
{
    std::cout << "Chip8DA created" << std::endl;
}
    
bool Chip8Disassembler::getDisassembly(CpuAddress addr, std::string& disAssString)
{
    if (!decodeInstruction(addr))
    {
        return false;
    }
    
    disAssString = addressToString(addr);
    disAssString += "\t";
    disAssString += theDisassemblyText;
    return true;
}

std::string Chip8Disassembler::getText()
{
    return theDisassemblyText;
}

bool Chip8Disassembler::isInsEndOfBlock(CpuAddress addr)
{
    uint8_t firstOpCode;
    uint8_t secondOpCode;
    if(!getByteFromAddress(addr, &firstOpCode) ||
       !getByteFromAddress(addr+1, &secondOpCode) )
    {
        return true;
    }
    
    // RET is end of block
    if( (firstOpCode = 0x00) && (secondOpCode == 0xee) )
    {
        return true;
    }
    
    // JP, V0, addr
    if ( (firstOpCode & 0xf0) == 0xb0)
    {
        return true;
    }
    
    // EXIT
    if( (firstOpCode = 0x00) && (secondOpCode == 0xfd) )
    {
        return true;
    }
    
    
    // JP
    if ( (firstOpCode & 0xf0) == 0x10)
    {
        return true;
    }
    
    return false;
}

bool Chip8Disassembler::getByteFromAddress(CpuAddress address, uint8_t* retByte)
{
   MemoryDev* dev = theMemoryController->getDevice(address);
   
   if (dev == nullptr)
   {
      return false;
   }
   
   *retByte = dev->read8(address);
   return true;
}

void Chip8Disassembler::insClearScreen()
{
  theDisassemblyText = "cls";
}

void Chip8Disassembler::insReturnFromSub()
{
   theDisassemblyText = "ret";
}

void Chip8Disassembler::insJump(unsigned addr)
{
   ostringstream ss;
   ss << "jp loc_" << setfill('0') << setw(4) << hex << addr;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insCall(unsigned addr)
{
   if (addr == 0)
   {
      // Special internal meta instruction for a debugger breakpoint
      theDisassemblyText = "DEBUG_BREAK";
   }
   else
   {
      ostringstream ss;
      ss << "call loc_" << setfill('0') << setw(4) << hex << addr;

      theDisassemblyText = ss.str();
   }
}


void Chip8Disassembler::insSetIndexReg(unsigned addr)
{
   ostringstream ss;
   ss << "ld I, #" << setfill('0') << setw(4) << hex << addr;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insJumpWithOffset(unsigned addr)
{
   ostringstream ss;
   ss << "jp v0, #" << setfill('0') << setw(4) << hex << addr;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSkipNextIfRegEqConst(unsigned reg, unsigned val)
{
   ostringstream ss;
   ss << "se v" << hex << reg << ", #" << setfill('0') << setw(2) << hex << val;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSkipNextIfRegNotEqConst(unsigned reg, unsigned val)
{
   ostringstream ss;
   ss << "sne v" << hex << reg << ", #" << setfill('0') << setw(2) << hex << val;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSetReg(unsigned reg, unsigned val)
{
   ostringstream ss;
   ss << "ld v" << hex << reg << ", #" << setfill('0') << setw(2) << hex << val;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insAddReg(unsigned reg, unsigned val)
{
   ostringstream ss;
   ss << "add v" << hex << reg << ", #" << setfill('0') << setw(2) << hex << val;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insRandomNum(unsigned reg, unsigned mask)
{
   ostringstream ss;
   ss << "rnd v" << hex << reg << ", #" << setfill('0') << setw(2) << hex << mask;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSkipNextIfRegEq(unsigned reg1, unsigned reg2)
{
   ostringstream ss;
   ss << "se v" << hex << reg1 << ", v" << reg2;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSkipNextIfRegNotEq(unsigned reg1, unsigned reg2)
{
   ostringstream ss;
   ss << "sne v" << hex << reg1 << ", v" << reg2;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSetRegToRegVal(unsigned regToSet, unsigned regVal)
{
   ostringstream ss;
   ss << "ld v" << hex << regToSet << ", v" << regVal;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insOrReg(unsigned reg, unsigned otherReg)
{
   ostringstream ss;
   ss << "or v" << hex << reg << ", v" << otherReg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insAndReg(unsigned reg, unsigned otherReg)
{
   ostringstream ss;
   ss << "and v" << hex << reg << ", v" << otherReg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insXorReg(unsigned reg, unsigned otherReg)
{
   ostringstream ss;
   ss << "xor v" << hex << reg << ", v" << otherReg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insAddRegs(unsigned reg, unsigned otherReg)
{
   ostringstream ss;
   ss << "add v" << hex << reg << ", v" << otherReg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSubRegs(unsigned reg, unsigned otherReg)
{
   ostringstream ss;
   ss << "sub v" << hex << reg << ", v" << otherReg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSubRegsOtherOrder(unsigned reg, unsigned otherReg)
{
   ostringstream ss;
   ss << "subn v" << hex << reg << ", v" << otherReg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insRightShift(unsigned reg)
{
   ostringstream ss;
   ss << "shr v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insLeftShift(unsigned reg)
{
   ostringstream ss;
   ss << "shl v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSkipNextIfKeyPressed(unsigned reg)
{
   ostringstream ss;
   ss << "skp v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSkipNextIfKeyNotPressed(unsigned reg)
{
   ostringstream ss;
   ss << "sknp v" << hex << reg;

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insWaitForKeyPress(unsigned reg)
{
   ostringstream ss;
   ss << "ld v" << hex << reg << ", k";

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSetRegToDelayTimer(unsigned reg)
{
   ostringstream ss;
   ss << "ld v" << hex << reg << ", dt";

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSetDelayTimer(unsigned reg)
{
   ostringstream ss;
   ss << "ld dt, v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSetSoundTimer(unsigned reg)
{
   ostringstream ss;
   ss << "ld st, v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insAddRegToIndexReg(unsigned reg)
{
   ostringstream ss;
   ss << "add I, v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSetIndexToCharInReg(unsigned reg)
{
   ostringstream ss;
   ss << "ld f, v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insSetIndexMemoryToRegBcd(unsigned reg)
{
   ostringstream ss;
   ss << "ld b, v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insStoreRegsToIndexMemory(unsigned reg)
{
   ostringstream ss;
   ss << "ld [I], v" << hex << reg;

   theDisassemblyText = ss.str();
}


void Chip8Disassembler::insLoadRegsFromIndexMemory(unsigned reg)
{
   ostringstream ss;
   ss << "ld v" << hex << reg << ", [I]";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insStoreRegsToIndexMemoryXo(unsigned regStart, unsigned regStop)
{
   ostringstream ss;
   ss << "save v" << hex << regStart << " - v" << regStop;
   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insLoadRegsFromIndexMemoryXo(unsigned regStart, unsigned regStop)
{
   ostringstream ss;
   ss << "load v" << hex << regStart << " - v" << regStop;
   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insDrawSprite(unsigned xReg, unsigned yReg, unsigned height)
{
   ostringstream ss;
   ss << "drw v" << hex << xReg << ", v" << yReg << ", #" << height;

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insBad(unsigned opCode)
{
   ostringstream ss;
   ss << "BAD OPCODE 0x" << setfill('0') << setw(4) << hex << opCode;

   theDisassemblyText = ss.str();
}

// Super Chip-8 Instructions
void Chip8Disassembler::insScrollDown(unsigned char numLines)
{
   ostringstream ss;
   ss << "scd " << (int) numLines;

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insScrollUp(unsigned char numLines)
{
   ostringstream ss;
   ss << "scu " << (int) numLines;

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insScrollLeft()
{
   ostringstream ss;
   ss << "scl";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insScrollRight()
{
   ostringstream ss;
   ss << "scr";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insQuitEmulator()
{
   ostringstream ss;
   ss << "exit";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insEnterLowResMode()
{
   ostringstream ss;
   ss << "low";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insEnterHighResMode()
{
   ostringstream ss;
   ss << "high";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insSaveHp48Flags(unsigned char reg)
{
   ostringstream ss;
   ss << "ld r, v" << Utils::toHexNibble(reg) << " ; superchip-8 HP flag instruction";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insLoadHp48Flags(unsigned char reg)
{
   ostringstream ss;
   ss << "ld v" << Utils::toHexNibble(reg) << ", r ; superchip-8 flag instruction";

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insSetIndexToHiResCharInReg(unsigned char reg)
{
   ostringstream ss;
   ss << "ld hf, v" << hex << (unsigned int) reg;

   theDisassemblyText = ss.str();
}

void Chip8Disassembler::insGraphicsPlane(unsigned char planeNum)
{
   ostringstream ss;
   ss << "plane " << hex << (unsigned int) planeNum << " ; octo x0 instruction";

   theDisassemblyText = ss.str();
}

