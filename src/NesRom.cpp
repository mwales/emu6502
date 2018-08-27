#include "NesRom.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "Logger.h"

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
   thePlayChoicePromData(nullptr)

{
   NES_ROM_DEBUG() << "Created a iNES ROM device: " << name;

   memset(&theHeaderBytes, 0, sizeof(struct INesHeader));
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

   return 0;
   //return theData[absAddr - theAddress];
}

bool NesRom::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   return true;
}

uint16_t NesRom::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   return 0;
   //return *retData;
}

bool NesRom::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   return true;
}

// ROM configuration flags
#define ROM_FILE_CONFIG 0x01
#define ROM_CONFIG_DONE  ROM_FILE_CONFIG

bool NesRom::isFullyConfigured()
{
   return (theConfigFlags == ROM_CONFIG_DONE);
}

std::vector<std::string> NesRom::getIntConfigParams()
{
   std::vector<std::string> retVal;
   return retVal;
}

std::vector<std::string> NesRom::getStringConfigParams()
{
   std::vector<std::string> retVal;
   retVal.push_back("romFilename");
   return retVal;
}

void NesRom::setIntConfigValue(std::string paramName, int value)
{

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

//      int fileSize = SDL_RWsize(f);
//      if (fileSize == -1)
//      {
//         errorOut = "Error getting the file size: ";
//         errorOut += SDL_GetError();
//         SDL_RWclose(f);
//         return retVal;
//      }

//      retVal.reserve(fileSize);

//      char buf[4097];
//      int bytesToRead = fileSize;
//      while(bytesToRead)
//      {
//         int bytesToReadIntoBuf = 4096;
//         if ( bytesToReadIntoBuf > bytesToRead)
//            bytesToReadIntoBuf = bytesToRead;

//         int numBytes = SDL_RWread(f, buf, 1, bytesToReadIntoBuf);

//         if (numBytes == 0)
//            break;

//         buf[numBytes] = 0;
//         retVal += buf;

//         bytesToRead -= numBytes;
//      }

//      SDL_RWclose(f);


//      int32_t sizeStatus =  lseek(fd, 0, SEEK_END);
//      if (sizeStatus < 0)
//      {
//         NES_ROM_WARNING() << "ROM INIT ERROR: Couldn't seek to the end of the ROM";
//         close(fd);
//         return;
//      }

//      if (sizeStatus > UINT16_MAX)
//      {
//         NES_ROM_WARNING() << "ROM INIT ERROR: ROM File too large for 6502 memory space";
//         close(fd);
//         return;
//      }

//      if (sizeStatus > (UINT16_MAX - theAddress + 1))
//      {
//         NES_ROM_WARNING() << "ROM INIT ERROR: ROM file will not fit in the memory region.  Space ="
//                       << Utils::toHex16(UINT16_MAX - theAddress + 1) << ", ROM size = "
//                       << Utils::toHex16(sizeStatus);
//         close(fd);
//         return;
//      }

//      theSize = sizeStatus;

//      if (theData != nullptr)
//      {
//         delete[] theData;
//      }

//      theData = new uint8_t[theSize];

//      lseek(fd, 0, SEEK_SET);

//      uint16_t bytesLeft = theSize;
//      int bytesReadCumulative = 0;
//      while(bytesLeft)
//      {
//         int bytesRead = read(fd, &theData[bytesReadCumulative], bytesLeft);

//         if (bytesRead <= 0)
//         {
//            NES_ROM_WARNING() << "ROM INIT ERROR: Error reading the ROM contents";
//            close(fd);
//            return;
//         }

//         bytesLeft -= bytesRead;
//         bytesReadCumulative += bytesRead;
//      }

//      NES_ROM_DEBUG() << "ROM INITIALIZED: " << theRomFile << " (" << theSize << " bytes) "
//                  << addressToString(theAddress) << "-" << addressToString(theAddress + theSize);
//      close(fd);
}

void NesRom::dumpHeaderInfo()
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

uint8_t NesRom::getMapperNumber()
{
    return 0;
}


uint32_t NesRom::getPrgRomSize()
{
    return theHeaderBytes.thePrgRomSizeBlocks * 16 * 1024;
}

uint32_t NesRom::getChrRomSize()
{
    return theHeaderBytes.theChrRomSizeBlocks * 1024 * 8;
}
