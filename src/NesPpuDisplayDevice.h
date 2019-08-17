#ifndef NESPPUDISPLAYDEVICE_H
#define NESPPUDISPLAYDEVICE_H

#include <stdint.h>

#include "DisplayDevice.h"
#include "Cpu6502Defines.h"


class NesPpuDisplayDevice : public DisplayDevice
{
public:
   NesPpuDisplayDevice();

   virtual uint8_t read8(CpuAddress absAddr) override;

   virtual bool write8(CpuAddress absAddr, uint8_t val) override;

   virtual uint16_t read16(CpuAddress absAddr) override;

   virtual bool write16(CpuAddress absAddr, uint16_t val) override;

   virtual void startDisplay() override;

protected:

   enum ppuRegNames
   {
      PPUCTRL,
      PPUMASK,
      PPUSTATUS,
      OAMADDR,
      OAMDATA,
      PPUSCROLL,
      PPUADDR,
      PPUDATA
   };

   std::string regNameToString(int offset) const;

   static const CpuAddress PPU_BASE_ADDR = 0x2000;

   static const CpuAddress PPU_ADDR_MASK = 0x0008;

   static const int NUM_PPU_REGISTERS = 8;

   static const int SPRITE_RAM_SIZE = 0x100;

   static const CpuAddress VRAM_SIZE = 0x4000;

   uint8_t thePpuRegisters[8];



   uint8_t theSpriteRam[SPRITE_RAM_SIZE];

   uint8_t theVRam[VRAM_SIZE];
};

#endif // NESPPUDISPLAYDEVICE_H
