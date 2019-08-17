#include "NesRom.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>

#include <SDL.h>

#include "Logger.h"

#include "NRomMapper.h"

#define ROM_TRACE

#ifdef ROM_TRACE
   #define NES_ROM_DEBUG    LOG_DEBUG
   #define NES_ROM_WARNING  LOG_WARNING
#else
   #define NES_ROM_DEBUG    if(0) LOG_DEBUG
   #define NES_ROM_WARNING  if(0) LOG_WARNING
#endif

// Static methods
std::string NesRom::getTypeName()
{
   return "iNES";
}

MemoryDev* nesRomDeviceConstructor(std::string name)
{
   return new NesRom(name);
}

MemoryDeviceConstructor NesRom::getMDC()
{
   return nesRomDeviceConstructor;
}

NesRom::NesRom(std::string name):
   MemoryDev(name),
   theRomFile(""),
   theConfigFlags(0),
   theTrainerData(nullptr),
   thePrgRomData(nullptr),
   theChrRomData(nullptr),
   thePlayChoiceInstRomData(nullptr),
   thePlayChoicePromData(nullptr),
   theMapper(nullptr)

{
   NES_ROM_DEBUG() << "Created a iNES ROM device: " << name;

   memset(&theHeaderBytes, 0, sizeof(struct INesHeader));

   theAddress = 0x6000;
   theSize = 0xa000;
}

NesRom::~NesRom()
{
   /// @todo Delete lots of memory here, maybe!
}

uint8_t NesRom::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      return 0;
   }

   if (theMapper == nullptr)
   {
       NES_ROM_WARNING() << "NES ROM read8 failure @" << addressToString(absAddr) << " - no mapper found";
       return 0;
   }

   return theMapper->read8(absAddr);
}

bool NesRom::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   if (theMapper == nullptr)
   {
       NES_ROM_WARNING() << "NES ROM write8 failure @" << addressToString(absAddr) << " - no mapper found";
       return false;
   }

   theMapper->write8(absAddr, val);
   return true;
}

uint16_t NesRom::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   if (theMapper == nullptr)
   {
       NES_ROM_WARNING() << "NES ROM read16 failure @" << addressToString(absAddr) << " - no mapper found";
       return 0;
   }

   uint8_t secondByte = theMapper->read8(absAddr);
   uint8_t firstByte = theMapper->read8(absAddr + 1);

   uint16_t retVal = firstByte;
   retVal <<= 8;
   retVal += secondByte;

   return retVal;
}

bool NesRom::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   if (theMapper == nullptr)
   {
       NES_ROM_WARNING() << "NES ROM write16 failure @" << addressToString(absAddr) << " - no mapper found";
       return false;
   }

   uint8_t firstByte = (val >> 8) & 0xff;
   uint8_t secondByte = val & 0xff;
   theMapper->write8(absAddr, secondByte);
   theMapper->write8(absAddr+1, firstByte);

   return true;
}

// ROM configuration flags
#define ROM_FILE_CONFIG 0x01
#define ROM_CONFIG_DONE  ROM_FILE_CONFIG

bool NesRom::isFullyConfigured() const
{
   return (theConfigFlags == ROM_CONFIG_DONE);
}

std::vector<std::string> NesRom::getIntConfigParams() const
{
   std::vector<std::string> retVal;
   return retVal;
}

std::vector<std::string> NesRom::getStringConfigParams() const
{
   std::vector<std::string> retVal;
   retVal.push_back("romFilename");
   return retVal;
}

void NesRom::setIntConfigValue(std::string paramName, int value)
{
    // Empty
}

void NesRom::setStringConfigValue(std::string paramName, std::string value)
{
   if (paramName == "romFilename")
   {
      theRomFile = value;
      theConfigFlags |= ROM_FILE_CONFIG;
      NES_ROM_DEBUG() << "iNES ROM " << theName << " filename = " << theRomFile;
   }
}

void NesRom::resetMemory()
{
      if (!isFullyConfigured())
      {
         LOG_FATAL() << "ROM " << theName << " not fully configured during reset";
      }

      int fd = open(theRomFile.c_str(), O_RDONLY);

      if (fd <= 0)
      {
         NES_ROM_WARNING() << "ROM INIT ERROR: Couldn't open ROM file " << theRomFile;
         return;
      }

      SDL_RWops* f = SDL_RWFromFile(theRomFile.c_str(), "r");

      if (f == NULL)
      {
         NES_ROM_WARNING() << "ROM INIT ERROR: Couldn't open ROM file (" << theRomFile << ") "
                           << SDL_GetError();
         return;
      }

      size_t numHeaderBytesRead = 0;
      while (numHeaderBytesRead < sizeof(struct INesHeader))
      {
         int bytesRead = SDL_RWread(f, &theHeaderBytes + numHeaderBytesRead,
                                    1, sizeof(struct INesHeader) - numHeaderBytesRead);
         if (bytesRead == 0)
         {
            // File not even long enough to contain the header
            LOG_FATAL() << "Failed to read iNES file " << theRomFile << ", " << numHeaderBytesRead
                        << " bytes of header read of " << sizeof(struct INesHeader);
            SDL_RWclose(f);
            return;
         }

         numHeaderBytesRead += bytesRead;
      }

      dumpHeaderInfo();

      if (theHeaderBytes.theFlagSix.theTrainerPresentFlag)
      {
          // The emulator doesn't care about trainer data, just read past this
          uint8_t garbageBuffer[512];
          int nbr = Utils::readUntilEof(garbageBuffer, 512, f);

          if (nbr != 512)
          {
              NES_ROM_WARNING() << "EOF encountered after reading " << nbr << " bytes of trainer data";
              SDL_RWclose(f);
              return;
          }

          NES_ROM_DEBUG() << "Read and discarded trainer data from NES ROM" << theName;
      }
      else
      {
          NES_ROM_DEBUG() << "No trainer data to read from NES ROM " << theName;
      }

      switch(getMapperNumber())
      {
      case 0:
          NES_ROM_DEBUG() << "Creating NROM mapper for NES ROM " << theName;
          theMapper = new NRomMapper(&theHeaderBytes, f);

          break;

      default:
          NES_ROM_WARNING() << "Mapper type " << Utils::toHex8(getMapperNumber()) << " not implemented";

      }

      SDL_RWclose(f);

}


bool NesRom::specifiesStartAddress() const
{
   if (!isAbsAddressValid(theAddressOfPcStart) || !isAbsAddressValid(theAddressOfPcStart + 1))
   {
      NES_ROM_DEBUG() << "specifiesStartAddress() for NesRom returning false becasuse address"
                      << addressToString(theAddressOfPcStart) << "not in ROM space";
      return false;
   }

   if (theMapper == nullptr)
   {
       NES_ROM_DEBUG() << "specifiesStartAddress() for NesRom returning false because mapper null";
       return false;
   }

   return true;
}

CpuAddress NesRom::getStartPcAddress() const
{
   if (!specifiesStartAddress())
   {
      NES_ROM_WARNING() << "NesRom::getStartPcAddress called, but NesRom can't specify address";
      return 0;
   }

   uint8_t secondByte = theMapper->read8(theAddressOfPcStart);
   uint8_t firstByte = theMapper->read8(theAddressOfPcStart + 1);

   CpuAddress retVal = firstByte;
   retVal <<= 8;
   retVal += secondByte;

   NES_ROM_DEBUG() << "Start address from ROM:" << addressToString(retVal);

   return retVal;
}

void NesRom::dumpHeaderInfo() const
{
    char magicBytes[4];
    magicBytes[0] = theHeaderBytes.theMagicNesBytes[0];
    magicBytes[1] = theHeaderBytes.theMagicNesBytes[1];
    magicBytes[2] = theHeaderBytes.theMagicNesBytes[2];
    magicBytes[3] = 0;

    NES_ROM_DEBUG() << "Header Structure Size: " << sizeof(struct INesHeader) << " bytes";

    NES_ROM_DEBUG() << "Magic Bytes: " << magicBytes << " "
                    << Utils::toHex8(theHeaderBytes.theMagicNesBytes[0]) << ", "
                    << Utils::toHex8(theHeaderBytes.theMagicNesBytes[1]) << ", "
                    << Utils::toHex8(theHeaderBytes.theMagicNesBytes[2]) << ", "
                    << Utils::toHex8(theHeaderBytes.theMagicNesBytes[3]);

    NES_ROM_DEBUG() << "PRG ROM Num Blocks: " << (int) theHeaderBytes.thePrgRomSizeBlocks << " ("
                    << getPrgRomSize() << " bytes)";

    NES_ROM_DEBUG() << "CHR ROM Num Blocks: " << (int) theHeaderBytes.theChrRomSizeBlocks << " ("
                    << getChrRomSize() << " bytes)";

    uint8_t mapperNumber = (theHeaderBytes.theFlagSeven.theMapperUpperNibble << 4) +
                           theHeaderBytes.theFlagSix.theMapperLowNibble;
    NES_ROM_DEBUG() << "Mapper Number: " << Utils::toHex8(mapperNumber);

    NES_ROM_DEBUG() << "Flag 6: " << Utils::toHex8(theHeaderBytes.theFlagSix.theWholeRegister);
    NES_ROM_DEBUG() << "  Mirror: " << (theHeaderBytes.theFlagSix.theMirroringFlag ? "1 = vertical" : "0 = horizontal");
    NES_ROM_DEBUG() << "  Battery Backup PGR RAM: " << (theHeaderBytes.theFlagSix.theBatteryBackupRamFlag ? "Yes" : "No");
    NES_ROM_DEBUG() << "  512 Byte Trainer: " << (theHeaderBytes.theFlagSix.theTrainerPresentFlag ? "Yes" : "No");
    NES_ROM_DEBUG() << "  Ignore Mirror Bit: " << (theHeaderBytes.theFlagSix.the4ScreenVRamFlag ? "1: Ignore mirroring / 4 screen VRAM" : "0: No");

    NES_ROM_DEBUG() << "Flag 7: " << Utils::toHex8(theHeaderBytes.theFlagSeven.theWholeRegister);
    NES_ROM_DEBUG() << "  VS Unisystem: " << (theHeaderBytes.theFlagSeven.theVsUnisystemFlag ? "Yes" : "No");
    NES_ROM_DEBUG() << "  PlayChoice-10 (8KB of Hint Screen Data): " << (theHeaderBytes.theFlagSeven.thePlayChoice10Flag ? "Yes" : "No");
    NES_ROM_DEBUG() << "  Format (2 for NES 2.0 format): " << (int) theHeaderBytes.theFlagSeven.theHeaderFormatFlag;

    NES_ROM_DEBUG() << "Flag 9: " << Utils::toHex8(theHeaderBytes.theFlagNine.theWholeRegister);
    NES_ROM_DEBUG() << "  TV System: " << (theHeaderBytes.theFlagNine.theTvSystem ? "1 = PAL" : "0 = NTSC");

    NES_ROM_DEBUG() << "Flag 10: " << Utils::toHex8(theHeaderBytes.theFlagTen.theWholeRegister);
    NES_ROM_DEBUG() << "  TV system (0 = NTSC, 2 = PAL, 1/3 = compatible): "
                    << (int) theHeaderBytes.theFlagTen.theTvSystem;
    NES_ROM_DEBUG() << "  PRG RAM: " << (theHeaderBytes.theFlagTen.thePrgRamPresent ? "1 = present" : "0 = not present");
    NES_ROM_DEBUG() << "  Bus Conflicts: " << (theHeaderBytes.theFlagTen.theBusConflictsFlag ? "1 = has conflicts" : "0 = no conflicts");
}

uint8_t NesRom::getMapperNumber() const
{
    return 0;
}


uint32_t NesRom::getPrgRomSize() const
{
    return theHeaderBytes.thePrgRomSizeBlocks * 16 * 1024;
}

uint32_t NesRom::getChrRomSize() const
{
    return theHeaderBytes.theChrRomSizeBlocks * 1024 * 8;
}
