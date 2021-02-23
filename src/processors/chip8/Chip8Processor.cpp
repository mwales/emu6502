#include "EmulatorCommon.h"
#include "Chip8Processor.h"
//#include "audio_player.h"
#include "Chip8Disassembler.h"
#include "ctype.h"
#include "Logger.h"
#include <cstdlib>

#include "SDL.h"

#include "ProcessorFactory.h"
#include "MemoryDev.h"
#include "force_execute.h"
#include "Display.h"
#include "DisplayCommands.h"


#ifdef CHIP8_CPU_DEBUG
   #define C8DEBUG  LOG_DEBUG
   #define C8WARNING  LOG_WARNING
#else
   #define C8DEBUG    if(0) LOG_DEBUG
   #define C8WARNING  if(0) LOG_WARNING
#endif

#define NUM_REGISTERS      16
#define MAX_MEMORY         0x1000

#define SCREEN_MEMORY_ADDR 0x0f00
#define SCREEN_MEMORY_SIZE 0x0100

#define FONT_HEIGHT_STD        5
#define FONT_HEIGHT_HIRES      10

#define LOW_RES_WIDTH      64
#define LOW_RES_HEIGHT     32
#define HIGH_RES_WIDTH     128
#define HIGH_RES_HEIGHT    64


Chip8Processor::Chip8Processor(std::string instanceName):
   theIndexRegister(0),
   theTimerStartTickVal(0),
   theTimerValOriginal(0),
   theDisassembler(nullptr),
   theLowResFontsAddr(0x0),
   theHiResFontsAddr(0x0),
   theSoundEnabled(true),
   theEventQueue(nullptr)
{
   C8DEBUG() << "Creating an instance of" << instanceName;
   
   for (int i = 0; i < NUM_REGISTERS; i++)
   {
      theCpuRegisters.push_back(0);
   }
   
   thePc = 0x200;
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
   C8DEBUG() << "resetState() called";

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

   C8DEBUG() << "Going to start the display";

   theDisplay.resetDisplay();

}

uint8_t mapSdlKeyCodeToChip8(SDL_Keysym ks)
{
   // Assuming key layout of:
   // 123C
   // 456D
   // 789E
   // A0BF

   SDL_Keycode keycode = ks.sym;

   // Top row
   if (keycode == SDLK_1)
   {
      return 1;
   }
   if (keycode == SDLK_2)
   {
      return 2;
   }
   if (keycode == SDLK_3)
   {
      return 3;
   }
   if (keycode == SDLK_4)
   {
      return 0xc;
   }

   // Second row
   if (keycode == SDLK_q)
   {
      return 4;
   }
   if (keycode == SDLK_w)
   {
      return 5;
   }
   if (keycode == SDLK_e)
   {
      return 6;
   }
   if (keycode == SDLK_r)
   {
      return 0xd;
   }

   // Third row
   if (keycode == SDLK_a)
   {
      return 7;
   }
   if (keycode == SDLK_s)
   {
      return 8;
   }
   if (keycode == SDLK_d)
   {
      return 9;
   }
   if (keycode == SDLK_f)
   {
      return 0xe;
   }

   // Last row
   if (keycode == SDLK_z)
   {
      return 0xa;
   }
   if (keycode == SDLK_x)
   {
      return 0;
   }
   if (keycode == SDLK_c)
   {
      return 0xb;
   }
   if (keycode == SDLK_v)
   {
      return 0xf;
   }

   return 0xff;
}

void Chip8Processor::checkDisplayEvents()
{
   // process display events
   int numBytesEvent;
   DisplayEvent ev;
   bool success;
   success = Display::getInstance()->getEventQueue()->tryReadMessage(&numBytesEvent,
                                                                     (char*) &ev,
                                                                     sizeof(DisplayEvent));
   if(!success)
   {
      return;
   }

   if (ev.id == KEY_STATE_CHANGE)
   {
      uint8_t keyChar = mapSdlKeyCodeToChip8(ev.data.DeKeyStateChange.keyCode);
      if (keyChar == 0xff)
      {
         // Invalid key pressed, ignore
         // C8DEBUG() << "Invalid key pressed";
         return;
      }

      if (ev.data.DeKeyStateChange.isDown)
      {
         theKeysDown.emplace(keyChar);
      }
      else
      {
         theKeysDown.erase(keyChar);
      }
   }
}

bool Chip8Processor::step()
{
   if (thePc > (0x1000 - 2))
   {
      C8DEBUG() << "Instruction address " << addressToString(thePc) << " out of bounds";
      return false;
   }

   checkDisplayEvents();

   if (!decodeInstruction(thePc))
   {
      std::cout << "Error executing instruction:" << std::endl;
      
      return false;
   }
   
   thePc += 2;
   theInstructionsExecuted++;
   return true;
}

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

uint8_t Chip8Processor::getDelayTimer()
{
    // Special case, no one ever started the timer
    if (theTimerValOriginal == 0)
    {
       return 0;
    }

    uint32_t currentTimerVal = SDL_GetTicks();
    uint32_t timeDiffMs;

    if(currentTimerVal < theTimerStartTickVal)
    {
       // Special case, timer rolled over!
       timeDiffMs = 0xffffffff - theTimerStartTickVal + currentTimerVal;
    }
    else
    {
       timeDiffMs = currentTimerVal - theTimerStartTickVal;
    }

    double numberOfChip8Ticks = (double) timeDiffMs / 16.666667;
    double regValueRaw = theTimerValOriginal - numberOfChip8Ticks;

    if (regValueRaw < 0)
    {
       return 0;
    }
    else if (regValueRaw > 0xff)
    {
       return 0xff;
    }
    else
    {
       return (uint8_t) regValueRaw;
    }
}

void Chip8Processor::insClearScreen()
{
   theDisplay.clearScreen();
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
   bool waitingForKey = theKeysDown.empty();

   if (!waitingForKey)
   {
      // Get a pressed key
      theCpuRegisters[reg] = *(theKeysDown.begin());
   }

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
   theTimerStartTickVal = SDL_GetTicks();
   theTimerValOriginal = theCpuRegisters[reg];
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
   theIndexRegister = theCpuRegisters[reg] * FONT_HEIGHT_STD + theLowResFontsAddr;
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

   if (!theMemoryController->write8(theIndexRegister, val/100))
   {
      C8WARNING() << "BCD operation accessed invalid memory address";
      theStopFlag = true;
      return;
   }

   val = val % 100;

   if (!theMemoryController->write8(theIndexRegister + 1, val/10))
   {
      C8WARNING() << "BCD operation accessed invalid memory address";
      theStopFlag = true;
      return;
   }

   val = val % 10;

   if (!theMemoryController->write8(theIndexRegister + 2, val))
   {
      C8WARNING() << "BCD operation accessed invalid memory address";
      theStopFlag = true;
   }
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
      if (!theMemoryController->write8(theIndexRegister + i, theCpuRegisters[i]))
      {
         C8WARNING() << "Store operation accessed invalid memory address";
         theStopFlag = true;
      }
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
      uint8_t val;
      if (theMemoryController->read8(theIndexRegister + i, &val))
      {
         theCpuRegisters[i] = val;
      }
      else
      {
         C8WARNING() << "Load operation accessed invalid memory address";
         theStopFlag = true;
      }
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

      if (!theMemoryController->write8(theIndexRegister + i, theCpuRegisters[regIndex]))
      {
         C8WARNING() << "StoreXo operation accessed invalid memory address";
         theStopFlag = true;
      }

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

      uint8_t val;
      if (theMemoryController->read8(theIndexRegister + i, &val))
      {
         theCpuRegisters[regIndex] = val;
      }
      else
      {
         C8WARNING() << "LoadXo operation accessed invalid memory address";
         theStopFlag = true;
      }

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
   std::vector<uint8_t> spriteData;
   bool collision;

   int numBytesRequired = height;
   if (height == 0)
   {

     if (theDisplay.isHighResMode())
     {
        // Sprite is 16-bits wide, 16 bits tall, 32 total bytes
        numBytesRequired = 32;
     }
     else
     {
        // Normal-res, sprite of 0 rows = 16 rows of 8 bits, 16 total bytes
        numBytesRequired = 16;
     }
   }

   // For XO mode, there are multiple bit planes, see how many bit planes are active
   numBytesRequired *= theDisplay.getNumBitPlanes();

   // Does the memory fit the sprite data required
   if (theIndexRegister + numBytesRequired >= MAX_MEMORY)
   {
      C8DEBUG() << "Index register out of valid memory range for the draw sprite instruction.  Index = "
               << addressToString(theIndexRegister) << "and" << Utils::toHex16(numBytesRequired)
               << "bytes are required.  CPU address is" << Utils::toHex16(thePc, 16);
      theStopFlag = true;
      return;
   }

   for(int i = 0; i < numBytesRequired; i++)
   {

      uint8_t val = 0;
      if (!theMemoryController->read8(theIndexRegister + i, &val))
      {
         C8WARNING() << "Load operation accessed invalid memory address";
         theStopFlag = true;
      }
      spriteData.push_back(val);

   }

   if (height > 0)
   {
      collision = theDisplay.drawSprite(theCpuRegisters[xReg] % theDisplay.getScreenWidth(),
                                        theCpuRegisters[yReg] % theDisplay.getScreenHeight(),
                                        height, spriteData);
   }
   else
   {
      // height = 0, special super chip-8 form of instruction
      if (theDisplay.isHighResMode())
      {
         // Draw super sprite!
         collision = theDisplay.drawSuperSprite(theCpuRegisters[xReg] % theDisplay.getScreenWidth(),
                                                theCpuRegisters[yReg] % theDisplay.getScreenHeight(),
                                                spriteData);
      }
      else
      {
         // Draw 16 row sprite!
         collision = theDisplay.drawSprite(theCpuRegisters[xReg] % theDisplay.getScreenWidth(),
                                           theCpuRegisters[yReg] % theDisplay.getScreenHeight(),
                                           height, spriteData);
      }
   }

   if (collision)
      theCpuRegisters[0xf] = 1;
   else
      theCpuRegisters[0xf] = 0;

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

bool Chip8Processor::load5BytesHelper(CpuAddress& addr, uint8_t byteA, uint8_t byteB,
                      uint8_t byteC, uint8_t byteD, uint8_t byteE)
{
   bool retVal = true;
   retVal = retVal && theMemoryController->write8(addr++, byteA);
   retVal = retVal && theMemoryController->write8(addr++, byteB);
   retVal = retVal && theMemoryController->write8(addr++, byteC);
   retVal = retVal && theMemoryController->write8(addr++, byteD);
   retVal = retVal && theMemoryController->write8(addr++, byteE);
   return retVal;

}

void Chip8Processor::loadFonts()
{
   // Low res Font codes found at:  http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#dispcoords
   bool status = true;
   CpuAddress curFontAddr = 0x100;
   theLowResFontsAddr = curFontAddr;

   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x90, 0x90, 0x90, 0xf0); // 0
   status = status && load5BytesHelper(curFontAddr, 0x20, 0x60, 0x20, 0x20, 0x70);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x10, 0xf0, 0x80, 0xf0);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x10, 0xf0, 0x10, 0xf0);
   status = status && load5BytesHelper(curFontAddr, 0x90, 0x90, 0xf0, 0x10, 0x10);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x80, 0xf0, 0x10, 0xf0); // 5
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x80, 0xf0, 0x90, 0xf0);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x10, 0x20, 0x40, 0x40);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x90, 0xf0, 0x90, 0xf0);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x90, 0xf0, 0x10, 0xf0);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x90, 0xf0, 0x90, 0x90); // A
   status = status && load5BytesHelper(curFontAddr, 0xe0, 0x90, 0xe0, 0x90, 0xe0);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x80, 0x80, 0x80, 0xf0);
   status = status && load5BytesHelper(curFontAddr, 0xe0, 0x90, 0x90, 0x90, 0xe0);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x80, 0xf0, 0x80, 0xf0);
   status = status && load5BytesHelper(curFontAddr, 0xf0, 0x80, 0xf0, 0x80, 0x80); // F

   theHiResFontsAddr = curFontAddr;

   status = status && load5BytesHelper(curFontAddr, 0x3c, 0x66, 0xc3, 0x81, 0x81);
   status = status && load5BytesHelper(curFontAddr, 0x81, 0x81, 0xc3, 0x66, 0x3c); // 0
   status = status && load5BytesHelper(curFontAddr, 0x10, 0x30, 0x10, 0x10, 0x10);
   status = status && load5BytesHelper(curFontAddr, 0x10, 0x10, 0x10, 0x10, 0x7e);
   status = status && load5BytesHelper(curFontAddr, 0x3c, 0x66, 0x81, 0x01, 0x03);
   status = status && load5BytesHelper(curFontAddr, 0x06, 0x38, 0xc0, 0x80, 0xff);
   status = status && load5BytesHelper(curFontAddr, 0x7e, 0x83, 0x81, 0x01, 0x06);
   status = status && load5BytesHelper(curFontAddr, 0x06, 0x01, 0x81, 0x83, 0x7e);
   status = status && load5BytesHelper(curFontAddr, 0x04, 0x0c, 0x14, 0x24, 0x44);
   status = status && load5BytesHelper(curFontAddr, 0xff, 0x04, 0x04, 0x04, 0x04);
   status = status && load5BytesHelper(curFontAddr, 0xff, 0x80, 0x80, 0x80, 0xfe);
   status = status && load5BytesHelper(curFontAddr, 0xc2, 0x01, 0x01, 0x83, 0x7e); // 5
   status = status && load5BytesHelper(curFontAddr, 0x3e, 0x41, 0x81, 0x80, 0xbc);
   status = status && load5BytesHelper(curFontAddr, 0xc2, 0x81, 0x81, 0x81, 0x7e);
   status = status && load5BytesHelper(curFontAddr, 0xff, 0x01, 0x03, 0x06, 0x08);
   status = status && load5BytesHelper(curFontAddr, 0x10, 0x20, 0x60, 0x40, 0x80);
   status = status && load5BytesHelper(curFontAddr, 0x18, 0x24, 0x42, 0x42, 0x3c);
   status = status && load5BytesHelper(curFontAddr, 0x42, 0x81, 0x81, 0x81, 0x7e);
   status = status && load5BytesHelper(curFontAddr, 0x7e, 0x81, 0x81, 0x81, 0x43);
   status = status && load5BytesHelper(curFontAddr, 0x3d, 0x01, 0x81, 0x86, 0x7c);
   status = status && load5BytesHelper(curFontAddr, 0x18, 0x66, 0x42, 0x81, 0x81);
   status = status && load5BytesHelper(curFontAddr, 0xff, 0x81, 0x81, 0x81, 0x81); // A
   status = status && load5BytesHelper(curFontAddr, 0xfc, 0x82, 0x82, 0x82, 0xfc);
   status = status && load5BytesHelper(curFontAddr, 0x82, 0x81, 0x81, 0x81, 0xfe);
   status = status && load5BytesHelper(curFontAddr, 0x3c, 0x66, 0xc3, 0x81, 0x80);
   status = status && load5BytesHelper(curFontAddr, 0x80, 0x81, 0xc3, 0x66, 0x3c);
   status = status && load5BytesHelper(curFontAddr, 0xf8, 0x84, 0x82, 0x81, 0x81);
   status = status && load5BytesHelper(curFontAddr, 0x81, 0x81, 0x82, 0x84, 0xf8);
   status = status && load5BytesHelper(curFontAddr, 0xff, 0x80, 0x80, 0x80, 0x80);
   status = status && load5BytesHelper(curFontAddr, 0xfc, 0x80, 0x80, 0x80, 0xff);
   status = status && load5BytesHelper(curFontAddr, 0xff, 0x80, 0x80, 0x80, 0x80);
   status = status && load5BytesHelper(curFontAddr, 0xfc, 0x80, 0x80, 0x80, 0x80); // F

   if (!status)
   {
      C8WARNING() << "Error loading the fonts into RAM";
   }
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
   MemoryDev* dev = theMemoryController->getDevice(address);
   
   if (dev == nullptr)
   {
      return false;
   }
   
   *retByte = dev->read8(address);
   return true;
}


int Chip8Processor::getSaveStateLength()
{
   int returnVal = 0;

   returnVal += theMemoryController->getSaveStateLength();

   // todo get size of internal cpu state
   // todo get size of display state

   return returnVal;
}

bool Chip8Processor::saveState(uint8_t* buffer, uint32_t* bytesSaved)
{
   uint32_t savedBytes = 0;
   if (!theMemoryController->saveState(buffer, &savedBytes))
   {
      std::cout << "Error serializing the state of the memory controller" << std::endl;
      return false;
   }

   C8DEBUG() << "Successfully saved" << savedBytes << "bytes from memory controller";

   *bytesSaved += savedBytes;
   buffer += savedBytes;

   // get data internal cpu state
   // get data of display state

   return true;
}

bool Chip8Processor::loadState(uint8_t* buffer, uint32_t* bytesLoaded)
{
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
