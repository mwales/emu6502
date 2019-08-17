#include "NesPpuDisplayDevice.h"
#include "EmulatorConfig.h"
#include "Logger.h"

#ifdef PPUDEV_TRACE
   #define PPUDEV_DEBUG LOG_DEBUG
   #define PPUDEV_WARNING   LOG_WARNING
#else
   #define PPUDEV_DEBUG     if(0) LOG_DEBUG
   #define PPUDEV_WARNING   if(0) LOG_WARNING
#endif

NesPpuDisplayDevice::NesPpuDisplayDevice():
   DisplayDevice("NES PPU")
{
   theAddress = PPU_BASE_ADDR;
   theSize = 0x2000;    // It's the same 8 address mirrored all throughout this memory space

   memset(thePpuRegisters, 0, NUM_PPU_REGISTERS);
   memset(theSpriteRam, 0, SPRITE_RAM_SIZE);
   memset(theVRam, 0, VRAM_SIZE);
}

uint8_t NesPpuDisplayDevice::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
       LOG_FATAL() << "Address " << addressToString(absAddr) << "does not belong to NES PPU DEV";
       return 0;
   }

   int offset = absAddr & PPU_ADDR_MASK;

   PPUDEV_DEBUG() << "read8(" << regNameToString(offset) << ")";

   switch(offset)
   {
   case PPUSTATUS:



   case OAMDATA:




   case PPUDATA:

   // Cases for write-only registers
   case PPUCTRL:
   case PPUMASK:
   case OAMADDR:
   case PPUSCROLL:
   case PPUADDR:
      PPUDEV_WARNING() << "read8(" << regNameToString(offset) << ") is write-only register";
      return thePpuRegisters[offset];
   }

   return 0;
}

bool NesPpuDisplayDevice::write8(CpuAddress absAddr, uint8_t val)
{
   return false;
}

uint16_t NesPpuDisplayDevice::read16(CpuAddress absAddr)
{
   return 0;
}

bool NesPpuDisplayDevice::write16(CpuAddress absAddr, uint16_t val)
{
   return false;
}

void NesPpuDisplayDevice::startDisplay()
{

}

std::string NesPpuDisplayDevice::regNameToString(int offset) const
{
   switch(offset)
   {
   case PPUCTRL:
      return "PPUCTRL";
   case PPUMASK:
      return "PPUMASK";
   case PPUSTATUS:
      return "PPUSTATUS";
   case OAMADDR:
      return "OAMADDR";
   case OAMDATA:
      return "OAMDATA";
   case PPUSCROLL:
      return "PPUSCROLL";
   case PPUADDR:
      return "PPUADDR";
   case PPUDATA:
      return "PPUDATA";
   default:
      return "INVALID";
   }
}
