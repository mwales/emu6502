#include "EmulatorCommon.h"
#include "Chip8Processor.h"
//#include "audio_player.h"
#include "Chip8Disassembler.h"
#include "ctype.h"
#include "Logger.h"
#include <cstdlib>

#include "ProcessorFactory.h"
#include "force_execute.h"


#ifdef CHIP8_CPU_DEBUG
   #define C8DEBUG  LOG_DEBUG
   #define C8WARNING  LOG_WARNING
#else
   #define C89DEBUG    if(0) LOG_DEBUG
   #define C8WARNING  if(0) LOG_WARNING
#endif

#define NUM_REGISTERS      16
#define MAX_MEMORY         0x1000

#define SCREEN_MEMORY_ADDR 0x0f00
#define SCREEN_MEMORY_SIZE 0x0100

#define FONT_HEIGHT_STD        5
#define FONT_HEIGHT_HIRES      10




Chip8Processor::Chip8Processor(std::string instanceName):
   theHighResMode(false),
   theDisassembler(nullptr),
   theSoundEnabled(true)
{
   C8DEBUG() << "Creating an instance of" << instanceName;
   
   for (int i = 0; i < NUM_REGISTERS; i++)
   {
      theCpuRegisters.push_back(0);
   }

   loadFonts();
   

}

Chip8Processor::~Chip8Processor()
{
   C8DEBUG() << "Deleting instance of Chip8 CPU";
   
   if(theDisassembler)
   {
      delete theDisassembler;
      theDisassembler = nullptr;
   }
}


void Chip8Processor::resetState()
{
   // Reset the IP
   thePc = 0x0200;

   // Clear everything
   loadFonts();
   
   for(unsigned int i = 0; i < theCpuRegisters.size(); i++)
   {
      theCpuRegisters[i] = 0;
   }

   theIndexRegister = 0;
   theKeysDown.clear();
   theCpuStack.clear();

   //theScreen->clearScreen();
   //theScreen->hiResModeEnable(false);
   theHighResMode = false;

   // unsigned int loadAddr = thePc;
     

    // theDelayTimerExpiration = QDateTime::currentDateTime();
}

bool Chip8Processor::step()
{
   if (thePc > (0x1000 - 2))
   {
      C8DEBUG() << "Instruction address " << addressToString(thePc) << " out of bounds";
      return false;
   }

   decodeInstruction(thePc);
   return true;
}

void Chip8Processor::keyDown(unsigned char key)
{
//   theKeysLock.lock();
//   theKeysDown.insert(key);
//   theKeysLock.unlock();
}

void Chip8Processor::keyUp(unsigned char key)
{
//   theKeysLock.lock();
//   theKeysDown.remove(key);
//   theKeysLock.unlock();
}

//void Chip8Processor::setEmulationScreen(EmulationScreen* screen)
//{
//   theScreen = screen;
//}

unsigned char Chip8Processor::getRegister(unsigned char reg)
{
   if (reg >= 16)
   {
      C8WARNING() << "Illegal register value requested in Chip8Processor::getRegister()";
      return 0;
   }

   return theCpuRegisters[reg];
}

unsigned int Chip8Processor::getIP()
{
   return thePc;
}

unsigned int Chip8Processor::getIndexRegister()
{
   return theIndexRegister;
}

std::vector<unsigned int> Chip8Processor::getStack()
{
   return theCpuStack;
}

unsigned char Chip8Processor::getDelayTimer()
{
//   QDateTime curTime = QDateTime::currentDateTime();

//   if (curTime > theDelayTimerExpiration)
//   {
//      return 0;
//   }
//   else
//   {
//      int numMilliSecs = curTime.msecsTo(theDelayTimerExpiration);
//      return numMilliSecs / MS_PER_TIMER_COUNT;
//   }
    return 0;
}

void Chip8Processor::insClearScreen()
{
   //theScreen->clearScreen();
}

void Chip8Processor::insReturnFromSub()
{
   if (theCpuStack.empty())
   {
      C8DEBUG() << "Return statement with no return addresses on the stack";
      theStopFlag = true;
      return;
   }

   thePc = theCpuStack.back();
   theCpuStack.pop_back();
}

void Chip8Processor::insJump(unsigned addr)
{
   if (addr > 0x1000)
   {
      C8DEBUG() << "Jump to address" << addressToString(addr) << "is out of bounds";
      theStopFlag = true;
      return;
   }

   thePc = addr - 0x2;
}

void Chip8Processor::insCall(unsigned addr)
{
   if (addr > 0x1000)
   {
      C8DEBUG() << "Call to address" << addressToString(addr) << "is out of bounds";
      theStopFlag = true;
      return;
   }

   theCpuStack.push_back(thePc);
   thePc = addr - 0x2;
}

void Chip8Processor::insSetIndexReg(unsigned addr)
{
   if (addr > 0x1000)
   {
      C8DEBUG() << "Set Index Register address to" << addressToString(addr) << "is out of bounds";
      theStopFlag = true;
      return;
   }

   theIndexRegister = addr;
}

void Chip8Processor::insJumpWithOffset(unsigned addr)
{
   thePc = addr + theCpuRegisters[0] - 0x2;

   if (thePc > 0x1000)
   {
      C8DEBUG() << "jump with offset trying to jump out of memory";
      C8DEBUG() << "  addr=" << addr << " offset=" << theCpuRegisters[0];
      theStopFlag = true;
      return;
   }
}

void Chip8Processor::insSkipNextIfRegEqConst(unsigned reg, unsigned val)
{
   if (theCpuRegisters[reg] == val)
      thePc += 2;
}

void Chip8Processor::insSkipNextIfRegNotEqConst(unsigned reg, unsigned val)
{
   if (theCpuRegisters[reg] != val)
      thePc += 2;
}

void Chip8Processor::insSetReg(unsigned reg, unsigned val)
{
   theCpuRegisters[reg] = val;
}

void Chip8Processor::insAddReg(unsigned reg, unsigned val)
{
   theCpuRegisters[reg] += val;
}

void Chip8Processor::insRandomNum(unsigned reg, unsigned mask)
{
   unsigned int num = rand() % 256;
   theCpuRegisters[reg] = num & mask;
}

void Chip8Processor::insSkipNextIfRegEq(unsigned reg1, unsigned reg2)
{
   if (theCpuRegisters[reg1] == theCpuRegisters[reg2])
      thePc += 2;
}

void Chip8Processor::insSkipNextIfRegNotEq(unsigned reg1, unsigned reg2)
{
   if (theCpuRegisters[reg1] != theCpuRegisters[reg2])
      thePc += 2;
}

void Chip8Processor::insSetRegToRegVal(unsigned regToSet, unsigned regVal)
{
   theCpuRegisters[regToSet] = theCpuRegisters[regVal];
}

void Chip8Processor::insOrReg(unsigned reg, unsigned otherReg)
{
   theCpuRegisters[reg] = theCpuRegisters[reg] | theCpuRegisters[otherReg];
}

void Chip8Processor::insAndReg(unsigned reg, unsigned otherReg)
{
   theCpuRegisters[reg] = theCpuRegisters[reg] & theCpuRegisters[otherReg];
}

void Chip8Processor::insXorReg(unsigned reg, unsigned otherReg)
{
   theCpuRegisters[reg] = theCpuRegisters[reg] ^ theCpuRegisters[otherReg];
}

void Chip8Processor::insAddRegs(unsigned reg, unsigned otherReg)
{
   unsigned int res = theCpuRegisters[reg] + theCpuRegisters[otherReg];

   if (res > 255)
   {
      theCpuRegisters[0xf] = 1;
      theCpuRegisters[reg] = res % 256;
   }
   else
   {
      theCpuRegisters[0xf] = 0;
      theCpuRegisters[reg] = res;
   }

}

void Chip8Processor::insSubRegs(unsigned reg, unsigned otherReg)
{
   // I passed one of the Chip-8 ROM tests by adding equality to the following
   // test, but I'm not sure why this is
   if (theCpuRegisters[reg] >= theCpuRegisters[otherReg])
   {
      theCpuRegisters[0xf] = 1;
   }
   else
   {
      theCpuRegisters[0xf] = 0;
   }

   theCpuRegisters[reg] -= theCpuRegisters[otherReg];
}

void Chip8Processor::insSubRegsOtherOrder(unsigned reg, unsigned otherReg)
{
   // I passed one of the Chip-8 ROM tests by adding equality to the following
   // test, but I'm not sure why this is
   if (theCpuRegisters[reg] <= theCpuRegisters[otherReg])
   {
      theCpuRegisters[0xf] = 1;
   }
   else
   {
      theCpuRegisters[0xf] = 0;
   }

   theCpuRegisters[reg] = theCpuRegisters[otherReg] - theCpuRegisters[reg];
}

void Chip8Processor::insRightShift(unsigned reg)
{
   if (theCpuRegisters[reg] & 0x1)
   {
      theCpuRegisters[0xf] = 1;
   }
   else
   {
      theCpuRegisters[0xf] = 0;
   }

   theCpuRegisters[reg] = theCpuRegisters[reg] >> 1;
}

void Chip8Processor::insLeftShift(unsigned reg)
{
   if (theCpuRegisters[reg] & 0x80)
   {
      theCpuRegisters[0xf] = 1;
   }
   else
   {
      theCpuRegisters[0xf] = 0;
   }

   theCpuRegisters[reg] = theCpuRegisters[reg] << 1;
}

void Chip8Processor::insSkipNextIfKeyPressed(unsigned reg)
{
   if (theKeysDown.count(theCpuRegisters[reg]) > 0)
   {
      thePc += 2;
   }
}

void Chip8Processor::insSkipNextIfKeyNotPressed(unsigned reg)
{
   if (theKeysDown.count(theCpuRegisters[reg]) == 0)
   {
      thePc += 2;
   }
}

void Chip8Processor::insWaitForKeyPress(unsigned reg)
{
   // Not sure what this thing would do if more than one key pressed at a time, so we will just return the first item
   // in the set unless I come up with a better idea

   //theKeysLock.lock();

   bool waitingForKey = theKeysDown.empty();

   if (!waitingForKey)
   {
      // Get a pressed key
      theCpuRegisters[reg] = *(theKeysDown.begin());
   }

   //theKeysLock.unlock();

   if (waitingForKey)
   {
      // Rather than blocking, just move the IP back 2 and repeat this instruction over and over
      thePc -= 2;
   }

}

void Chip8Processor::insSetRegToDelayTimer(unsigned reg)
{
   theCpuRegisters[reg] = getDelayTimer();
}

void Chip8Processor::insSetDelayTimer(unsigned reg)
{
   //theDelayTimerExpiration = QDateTime::currentDateTime();
   //theDelayTimerExpiration = theDelayTimerExpiration.addMSecs(theCpuRegisters[reg] * MS_PER_TIMER_COUNT);
}

void Chip8Processor::insSetSoundTimer(unsigned reg)
{
   // I should be extending the sound duration if the sound timer set while it is already running, but I'm ignoring
   // that feature for now.  Sound is annoying enough as is.

//   if (theSoundEnabled)
//   {
//      AnnoyingSound* player = new AnnoyingSound();
//      player->setDuration(theCpuRegisters[reg] / 60.0);

//      QThreadPool::globalInstance()->start(player);
//   }
//   else
//   {
//      C8DEBUG() << "Sound Muted";
//   }
}

void Chip8Processor::insAddRegToIndexReg(unsigned reg)
{
   theIndexRegister += theCpuRegisters[reg] % 0x10000;
}

void Chip8Processor::insSetIndexToCharInReg(unsigned reg)
{
   // Low-res fonts start at address 0
   theIndexRegister = theCpuRegisters[reg] * FONT_HEIGHT_STD;
}

void Chip8Processor::insSetIndexMemoryToRegBcd(unsigned reg)
{
   if (theIndexRegister + 2 >= MAX_MEMORY)
   {
      C8DEBUG() << "Index register out of valid memory range for the BCD instruction.  Index = "
               << addressToString(theIndexRegister) << "and address is" << addressToString(thePc);
      theStopFlag = true;
      return;
   }

   unsigned char val = theCpuRegisters[reg];

   //theMemory[theIndexRegister] = val / 100;
   val = val % 100;

   //theMemory[theIndexRegister+1] = val / 10;
   val = val % 10;

  // theMemory[theIndexRegister+2] = val;
}

void Chip8Processor::insStoreRegsToIndexMemory(unsigned reg)
{
   if (theIndexRegister + reg >= MAX_MEMORY)
   {
      C8DEBUG() << "Index register out of valid memory range for the store regs instruction.  Index = "
               << addressToString(theIndexRegister) << "and address is" << addressToString(thePc);
      theStopFlag = true;
      return;
   }

   for(unsigned int i = 0; i <= reg; i++)
   {
      //theMemory[theIndexRegister + i] = theCpuRegisters[i];
   }
}

void Chip8Processor::insLoadRegsFromIndexMemory(unsigned reg)
{
   if (theIndexRegister + reg >= MAX_MEMORY)
   {
      C8DEBUG() << "Index register out of valid memory range for the load regs instruction.  Index = "
               << addressToString(theIndexRegister) << "and address is" << addressToString(thePc);
      theStopFlag = true;
      return;
   }

   for(unsigned int i = 0; i <= reg; i++)
   {
      //theCpuRegisters[i] = theMemory[theIndexRegister + i];
   }
}

void Chip8Processor::insStoreRegsToIndexMemoryXo(unsigned regStart, unsigned regStop)
{
   int numRegisters = 1;
   if (regStop > regStart)
   {
      // Instruction is inclusive of the registers, so we need to add 1
      numRegisters = regStop - regStart + 1;
   }
   else
   {
      // Making an assumption that we could wrap from vf to v0
      numRegisters = regStart - regStop + 1;
   }

   if (theIndexRegister + numRegisters >= MAX_MEMORY)
   {
      C8DEBUG() << "Index register out of valid memory range for the store-xo regs instruction.  Index = "
               << addressToString(theIndexRegister) << "and num registers "
               << Utils::toHex16(numRegisters);
      theStopFlag = true;
      return;
   }

   int regIndex = regStart & 0xf;
   for(int i = 0; i < numRegisters; i++)
   {

      //theMemory[theIndexRegister + i] = theCpuRegisters[regIndex];

      if (regStop > regStart)
      {
         regIndex++;
      }
      else
      {
         regIndex--;
      }
   }
}

void Chip8Processor::insLoadRegsFromIndexMemoryXo(unsigned regStart, unsigned regStop)
{
   int numRegisters = 1;
   if (regStop > regStart)
   {
      // Instruction is inclusive of the registers, so we need to add 1
      numRegisters = regStop - regStart + 1;
   }
   else
   {
      // Making an assumption that we could wrap from vf to v0
      numRegisters = regStart - regStop + 1;
   }

   if (theIndexRegister + numRegisters >= MAX_MEMORY)
   {
      C8DEBUG() << "Index register out of valid memory range for the load-xo regs instruction.  Index = "
               << addressToString(theIndexRegister) << "and num registers "
               << Utils::toHex16(numRegisters);
      theStopFlag = true;
      return;
   }

   int regIndex = regStart & 0xf;
   for(int i = 0; i < numRegisters; i++)
   {

      //theCpuRegisters[regIndex] = theMemory[theIndexRegister + i];

      if (regStop > regStart)
      {
         regIndex++;
      }
      else
      {
         regIndex--;
      }
   }
}



void Chip8Processor::insDrawSprite(unsigned xReg, unsigned yReg, unsigned height)
{
//   QVector<unsigned char> spriteData;
//   bool collision;

//   int numBytesRequired = height;
//   if (height == 0)
//   {
//     if (theHighResMode)
//     {
//        // Sprite is 16-bits wide, 16 bits tall, 32 total bytes
//        numBytesRequired = 32;
//     }
//     else
//     {
//        // Normal-res, sprite of 0 rows = 16 rows of 8 bits, 16 total bytes
//        numBytesRequired = 16;
//     }
//   }

//   // For XO mode, there are multiple bit planes, see how many bit planes are active
//   numBytesRequired *= theScreen->getNumBitPlanes();

//   // Does the memory fit the sprite data required
//   if (theIndexRegister + numBytesRequired >= MAX_MEMORY)
//   {
//      C8DEBUG() << "Index register out of valid memory range for the draw sprite instruction.  Index = "
//               << addressToString(theIndexRegister) << "and" << QString::number(numBytesRequired, 16)
//               << "bytes are required.  CPU address is" << QString::number(thePc, 16);
//      theStopFlag = true;
//      return;
//   }

//   for(unsigned int i = 0; i < numBytesRequired; i++)
//   {
//      spriteData.push_back(theMemory[theIndexRegister+i]);
//   }

//   if (height > 0)
//   {
//      collision = theScreen->drawSprite(theCpuRegisters[xReg] % (theHighResMode ? 128 : 64),
//                                        theCpuRegisters[yReg] % (theHighResMode ? 64 : 32),
//                                        spriteData);

//   }
//   else
//   {
//      // height = 0, special super chip-8 form of instruction
//      if (theHighResMode)
//      {
//         // Draw super sprite!
//         collision = theScreen->drawSuperSprite(theCpuRegisters[xReg] % 128,
//                                                theCpuRegisters[yReg] % 64,
//                                                spriteData);
//      }
//      else
//      {
//         // Draw 16 row sprite!
//         collision = theScreen->drawSprite(theCpuRegisters[xReg] % 64,
//                                           theCpuRegisters[yReg] % 32,
//                                           spriteData);
//      }
//   }

//   if (collision)
//      theCpuRegisters[0xf] = 1;
//   else
//      theCpuRegisters[0xf] = 0;
}

void Chip8Processor::insScrollDown(unsigned char numLines)
{
   //theScreen->shiftDown(numLines);
}

void Chip8Processor::insScrollUp(unsigned char numLines)
{
   //theScreen->shiftUp(numLines);
}

void Chip8Processor::insScrollLeft()
{
   //theScreen->shiftLeft();
}

void Chip8Processor::insScrollRight()
{
   //theScreen->shiftRight();
}

void Chip8Processor::insQuitEmulator()
{
   C8DEBUG() << "Quit Emulator Instruction";

   theStopFlag = true;

   // To not allow someone to step over the flag, reset the address
   thePc -= 2;
}

void Chip8Processor::insEnterLowResMode()
{
   //theScreen->hiResModeEnable(false);
   //theHighResMode = false;
}

void Chip8Processor::insEnterHighResMode()
{
  // theScreen->hiResModeEnable(true);
   //theHighResMode = true;
}

void Chip8Processor::insSaveHp48Flags(unsigned char)
{
   C8DEBUG() << __FUNCTION__;
}

void Chip8Processor::insLoadHp48Flags(unsigned char)
{
   C8DEBUG() << __FUNCTION__;
}

void Chip8Processor::insSetIndexToHiResCharInReg(unsigned char reg)
{
   // High-res fonts start at address the
   theIndexRegister = theHiResFontsAddr + theCpuRegisters[reg] * FONT_HEIGHT_HIRES;
}

void Chip8Processor::insGraphicsPlane(unsigned char planeNum)
{
   //theScreen->setPlane(planeNum);
}

void Chip8Processor::insBad(unsigned opCode)
{
   C8DEBUG() << "Bad opcode" << Utils::toHex16(opCode);
   theStopFlag = true;
}

void Chip8Processor::loadFonts()
{
   // Low res Font codes found at:  http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#dispcoords

//   theMemory << 0xf0 << 0x90 << 0x90 << 0x90 << 0xf0; // 0
//   theMemory << 0x20 << 0x60 << 0x20 << 0x20 << 0x70;
//   theMemory << 0xf0 << 0x10 << 0xf0 << 0x80 << 0xf0;
//   theMemory << 0xf0 << 0x10 << 0xf0 << 0x10 << 0xf0;
//   theMemory << 0x90 << 0x90 << 0xf0 << 0x10 << 0x10;
//   theMemory << 0xf0 << 0x80 << 0xf0 << 0x10 << 0xf0; // 5
//   theMemory << 0xf0 << 0x80 << 0xf0 << 0x90 << 0xf0;
//   theMemory << 0xf0 << 0x10 << 0x20 << 0x40 << 0x40;
//   theMemory << 0xf0 << 0x90 << 0xf0 << 0x90 << 0xf0;
//   theMemory << 0xf0 << 0x90 << 0xf0 << 0x10 << 0xf0;
//   theMemory << 0xf0 << 0x90 << 0xf0 << 0x90 << 0x90; // A
//   theMemory << 0xe0 << 0x90 << 0xe0 << 0x90 << 0xe0;
//   theMemory << 0xf0 << 0x80 << 0x80 << 0x80 << 0xf0;
//   theMemory << 0xe0 << 0x90 << 0x90 << 0x90 << 0xe0;
//   theMemory << 0xf0 << 0x80 << 0xf0 << 0x80 << 0xf0;
//   theMemory << 0xf0 << 0x80 << 0xf0 << 0x80 << 0x80; // F

//   theHiResFontsAddr = theMemory.size();

//   theMemory << 0x3c << 0x66 << 0xc3 << 0x81 << 0x81
//             << 0x81 << 0x81 << 0xc3 << 0x66 << 0x3c; // 0
//   theMemory << 0x10 << 0x30 << 0x10 << 0x10 << 0x10
//             << 0x10 << 0x10 << 0x10 << 0x10 << 0x7e;
//   theMemory << 0x3c << 0x66 << 0x81 << 0x01 << 0x03
//             << 0x06 << 0x38 << 0xc0 << 0x80 << 0xff;
//   theMemory << 0x7e << 0x83 << 0x81 << 0x01 << 0x06
//             << 0x06 << 0x01 << 0x81 << 0x83 << 0x7e;
//   theMemory << 0x04 << 0x0c << 0x14 << 0x24 << 0x44
//             << 0xff << 0x04 << 0x04 << 0x04 << 0x04;
//   theMemory << 0xff << 0x80 << 0x80 << 0x80 << 0xfe
//             << 0xc2 << 0x01 << 0x01 << 0x83 << 0x7e; // 5
//   theMemory << 0x3e << 0x41 << 0x81 << 0x80 << 0xbc
//             << 0xc2 << 0x81 << 0x81 << 0x81 << 0x7e;
//   theMemory << 0xff << 0x01 << 0x03 << 0x06 << 0x08
//             << 0x10 << 0x20 << 0x60 << 0x40 << 0x80;
//   theMemory << 0x18 << 0x24 << 0x42 << 0x42 << 0x3c
//             << 0x42 << 0x81 << 0x81 << 0x81 << 0x7e;
//   theMemory << 0x7e << 0x81 << 0x81 << 0x81 << 0x43
//             << 0x3d << 0x01 << 0x81 << 0x86 << 0x7c;
//   theMemory << 0x18 << 0x66 << 0x42 << 0x81 << 0x81
//             << 0xff << 0x81 << 0x81 << 0x81 << 0x81; // A
//   theMemory << 0xfc << 0x82 << 0x82 << 0x82 << 0xfc
//             << 0x82 << 0x81 << 0x81 << 0x81 << 0xfe;
//   theMemory << 0x3c << 0x66 << 0xc3 << 0x81 << 0x80
//             << 0x80 << 0x81 << 0xc3 << 0x66 << 0x3c;
//   theMemory << 0xf8 << 0x84 << 0x82 << 0x81 << 0x81
//             << 0x81 << 0x81 << 0x82 << 0x84 << 0xf8;
//   theMemory << 0xff << 0x80 << 0x80 << 0x80 << 0x80
//             << 0xfc << 0x80 << 0x80 << 0x80 << 0xff;
//   theMemory << 0xff << 0x80 << 0x80 << 0x80 << 0x80
//             << 0xfc << 0x80 << 0x80 << 0x80 << 0x80; // F

}

// Controls to play, pause, reset the emulator

//void Chip8Processor::enableSound(bool enable)
//{
//   theSoundEnabled = enable;
//}

std::vector<std::string> Chip8Processor::getRegisterNames()
{
    std::vector<std::string> retVal;
    for(char singleLetter = '0'; singleLetter <= '9'; singleLetter++)
    {
        std::string regName = "V";
        regName += singleLetter;
        retVal.push_back(regName);
    }
    
    for(char singleLetter = 'A'; singleLetter <= 'F'; singleLetter++)
    {
        std::string regName = "V";
        regName += singleLetter;
        retVal.push_back(regName);
    }
    
    retVal.push_back("I");
    retVal.push_back("SP");
    retVal.push_back("SndTmr");
    retVal.push_back("DlyTmr");
    retVal.push_back("PC");
    
    return retVal;
}

uint32_t Chip8Processor::getRegisterValue(std::string regName)
{
    if (regName == "I")
    {
        return theIndexRegister;
    }
    
    if (regName == "SP")
    {
        return theCpuStack.size();
    }
    
    if (regName == "SndTmr")
    {
        return 1;
    }
    
    if (regName == "DlyTmr")
    {
        return 2;
    }
    
    if (regName == "PC")
    {
       return thePc;
    }
    
    if (regName[0] == 'V')
    {
        char registerLetter = toupper(regName[1]);
        
        if ( (registerLetter >= '0') && (registerLetter <= '9') )
        {
            return getRegister(registerLetter - '0');
        }
        
        if ( (registerLetter >= 'A') && (registerLetter <= 'F') )
        {
            return getRegister(registerLetter - 'A' + 10);
        }
    }
    
    LOG_WARNING() << "Invalid register name: " << regName;
    return 0;    
}

bool Chip8Processor::setRegisterValue(std::string regName, uint32_t value)
{
   
    if (regName == "I")
    {
        theIndexRegister = value & 0xfff;
        return true;
    }
    
    if (regName == "SP")
    {
        LOG_DEBUG() << "Set SP not implemented";
        return false;
    }
    
    if (regName == "SndTmr")
    {
        LOG_DEBUG() << "Set SndTimer not implemented";
        return false;
    }
    
    if (regName == "DlyTmr")
    {
        LOG_DEBUG() << "Set DlyTmr not implemented";
        return false;
    }
    
    if (regName == "PC")
    {
       thePc = value & 0xfff;
       return true;
    }
    
    if (regName[0] == 'V')
    {
        char registerLetter = toupper(regName[1]);
        
        if ( (registerLetter >= '0') && (registerLetter <= '9') )
        {
            theCpuRegisters[(registerLetter - '0') & 0xff] = value;
            return true;
        }
        
        if ( (registerLetter >= 'A') && (registerLetter <= 'F') )
        {
            theCpuRegisters[(registerLetter - 'A' + 10) & 0xff] = value;
            return true;
        }
    }
    
    LOG_WARNING() << "Invalid register name: " << regName;
    return false;    
}

uint32_t Chip8Processor::getInstructionLength(CpuAddress addr)
{
   // Fixed instruction length
   return 2;
}

int Chip8Processor::disassembleAddr(CpuAddress addr, std::string* listingDataRet)
{
   if (theDisassembler == nullptr)
   {
      theDisassembler = new Chip8Disassembler(theMemoryController);
   }
   
   if(!theDisassembler->getDisassembly(addr, *listingDataRet))
   {
      return -1;
   }
   
   if (theDisassembler->isInsEndOfBlock(addr))
   {
      return 0;
   }
   else
   {
      return getInstructionLength(addr);
   }
}

std::string Chip8Processor::getCpuName()
{
   return "chip8";
}

bool Chip8Processor::getByteFromAddress(CpuAddress address, uint8_t* retByte)
{
   /// @todo implement
   return false;
}

Processor* CreateChip8Processor(std::string instanceName)
{
   return new Chip8Processor(instanceName);
}

FORCE_EXECUTE(fe_chip8_cpu)
{
   ProcessorFactory::getInstance()->registerProcessorType("chip8", CreateChip8Processor);
}
