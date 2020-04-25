#ifndef NESROM_H
#define NESROM_H

#include "MemoryDev.h"
#include <iostream>
#include <stdint.h>

class Mapper;

#define INES_MIRRORING_HORIZONTAL 0
#define INES_MIRRORING_VERTICAL   1

typedef union Flag6Union
{
   uint8_t theWholeRegister;
   struct
   {
      uint8_t theMirroringFlag:1;
      uint8_t theBatteryBackupRamFlag:1;
      uint8_t theTrainerPresentFlag:1;
      uint8_t the4ScreenVRamFlag:1;
      uint8_t theMapperLowNibble:4;
   };
} Flag6;

typedef union Flag7Union
{
   uint8_t theWholeRegister;
   struct
   {
      uint8_t theVsUnisystemFlag:1;
      uint8_t thePlayChoice10Flag:1;
      uint8_t theHeaderFormatFlag:2;
      uint8_t theMapperUpperNibble:4;
   };
} Flag7;

#define INES_TV_SYSTEM_NTSC   0
#define INES_TV_SYSTEM_PAL    1

typedef union Flag9Union
{
   uint8_t theWholeRegister;
   struct
   {
      uint8_t theTvSystem:1;
      uint8_t theReserved:7;
   };
} Flag9;

typedef union Flag10Union
{
   uint8_t theWholeRegister;
   struct
   {
      uint8_t theTvSystem:2;
      uint8_t thePaddingA:2;
      uint8_t thePrgRamPresent:1;
      uint8_t theBusConflictsFlag:1;
      uint8_t thePaddingB:2;
   };
} Flag10;

struct INesHeader
{
   uint8_t theMagicNesBytes[4];

   uint8_t thePrgRomSizeBlocks;
   uint8_t theChrRomSizeBlocks;

   // Flags from nesdev wiki
   Flag6 theFlagSix;
   Flag7 theFlagSeven;

   uint8_t thePrgRamSizeBlocks;

   Flag9 theFlagNine;
   Flag10 theFlagTen;

   uint8_t thePaddingBytes[5];

} __attribute__((packed));

class NesRom : public MemoryDev
{
public:
   // Construction methods

   NesRom(std::string name);

   static MemoryDeviceConstructor getMDC();
   static std::string getTypeName();

   virtual ~NesRom();

   // Access methods

   virtual uint8_t read8(CpuAddress absAddr) override;

   virtual bool write8(CpuAddress absAddr, uint8_t val) override;

   virtual uint16_t read16(CpuAddress absAddr) override;

   virtual bool write16(CpuAddress absAddr, uint16_t val) override;

   // Configuration methods

   virtual bool isFullyConfigured() const override;

   virtual std::string getConfigTypeName() const;

   virtual std::vector<std::string> getIntConfigParams() const override;

   virtual std::vector<std::string> getStringConfigParams() const override;

   virtual void setIntConfigValue(std::string paramName, int value) override;
   virtual void setStringConfigValue(std::string paramName, std::string value) override;

   virtual void resetMemory() override;

   virtual bool specifiesStartAddress() const override;

   virtual CpuAddress getStartPcAddress() const override;

   void dumpHeaderInfo() const;

   uint8_t getMapperNumber() const;

   uint32_t getPrgRomSize() const;

   uint32_t getChrRomSize() const;

protected:

   std::string theRomFile;

   int theConfigFlags;

   struct INesHeader theHeaderBytes;

   uint8_t* theTrainerData;

   uint8_t* thePrgRomData;

   uint8_t* theChrRomData;

   uint8_t* thePlayChoiceInstRomData;

   uint8_t* thePlayChoicePromData;

   Mapper* theMapper;

   const CpuAddress theAddressOfPcStart = 0xfffc;


};

#endif // NESROM_H
