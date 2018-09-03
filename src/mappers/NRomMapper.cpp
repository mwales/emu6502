#include "NRomMapper.h"

const int NRomMapper::PRG_RAM_SIZE = 0x1000;  // 4KB
const int NRomMapper::PRG_ROM_SIZE = 0x4000;  // 16KB
const CpuAddress NRomMapper::PRG_RAM_ADDR = 0x6000;
const CpuAddress NRomMapper::PRG_ROM_ADDR = 0x8000;

NRomMapper::NRomMapper(struct INesHeader* inesHdr, SDL_RWops* romFile)
{
   MAPPER_DEBUG() << "NROM Mapper (00) instantiated";

   if ( (inesHdr->thePrgRomSizeBlocks != 1) && (inesHdr->thePrgRamSizeBlocks != 1) )
   {
       MAPPER_WARNING() << "NROM didn't configured for a block of PRG RAM or ROM";
   }

   thePrgRamRom = new uint8_t[PRG_RAM_SIZE];

   if (inesHdr->theChrRomSizeBlocks != 1)
   {
       MAPPER_WARNING() << "NROM should be configured for only one block of CHR ROM, not "
                        << Utils::toHex8(inesHdr->theChrRomSizeBlocks);
   }

   if ( (inesHdr->thePrgRomSizeBlocks != 1) && (inesHdr->thePrgRomSizeBlocks != 2) )
   {
       MAPPER_WARNING() << "NROM should be configured for only 1 or 2 blocks of PRG ROM, not "
                        << Utils::toHex8(inesHdr->thePrgRomSizeBlocks);
   }

   if (inesHdr->thePrgRomSizeBlocks == 1)
   {
       // PRG rom is 1  block (16KB)
       thePrgRom = new uint8_t[PRG_ROM_SIZE];

       int numBytes = Utils::readUntilEof(thePrgRom, PRG_ROM_SIZE, romFile);

       if (numBytes != PRG_ROM_SIZE)
       {
           MAPPER_WARNING() << "NES ROM file only had " << numBytes << " of an expected"
                            << PRG_ROM_SIZE << " bytes of PRG_ROM";
       }
       else
       {
           MAPPER_DEBUG() << "NROM Loaded with " << numBytes << " of PRG_ROM @ " << addressToString(PRG_ROM_ADDR);
       }

       theUpper8KMirrored = true;
       MAPPER_DEBUG() << "NROM PRG ROM memory mirrored at " << addressToString(PRG_ROM_ADDR + PRG_ROM_SIZE);
   }
   else
   {
       // We assume it is 2 blocks (32KB)
       thePrgRom = new uint8_t[2 * PRG_ROM_SIZE];

       int numBytes = Utils::readUntilEof(thePrgRom, 2 * PRG_ROM_SIZE, romFile);

       if (numBytes != 2 * PRG_ROM_SIZE)
       {
           MAPPER_WARNING() << "NES ROM file only had " << numBytes << " of an expected"
                            << 2 * PRG_ROM_SIZE << " bytes of PRG_ROM";
       }
       else
       {
           MAPPER_DEBUG() << "NROM Loaded with " << numBytes << " of PRG_ROM @ " << addressToString(PRG_ROM_ADDR);
       }

       // No mirroring
       theUpper8KMirrored = false;
   }

}

NRomMapper::~NRomMapper()
{
    MAPPER_DEBUG() << "NROM Mapper (00) being deleted";

    delete[] thePrgRamRom;
}

uint8_t NRomMapper::read8(CpuAddress address)
{
    if (address < PRG_RAM_ADDR)
    {
        MAPPER_WARNING() << "Invalid NROM mapper read @" << addressToString(address);
        return 0;
    }

    if (address < PRG_ROM_ADDR)
    {
        // Read of PRG RAM
        int offset = address & 0x0fff;
        return thePrgRamRom[offset];
    }

    // Must be a read of PrgRom
    int mask = (theUpper8KMirrored ? 0x1fff : 0x3fff);
    int offset = address & mask;
    return thePrgRom[offset];
}

void NRomMapper::write8(CpuAddress address, uint8_t value)
{
    if (address < PRG_RAM_ADDR)
    {
        MAPPER_WARNING() << "Invalid NROM mapper write (not mapper address) @" << addressToString(address);
        return;
    }

    if (address < PRG_ROM_ADDR)
    {
        // Write of PRG RAM
        int offset = address & 0x0fff;
        thePrgRamRom[offset] = value;
    }

    // Must be a write of PrgRom
    MAPPER_WARNING() << "Invalid NROM mapper write (read-only address) @" << addressToString(address);
    return;
}
