#ifndef NROMMAPPER_H
#define NROMMAPPER_H

#include <SDL2/SDL.h>
#include "Mapper.h"
#include "NesRom.h"


/**
 * NRomMapper is implementation of NES mapper 0x00 NROM
 */
class NRomMapper : public Mapper
{
public:
    NRomMapper(struct INesHeader* inesHdr, SDL_RWops* romFile);

    ~NRomMapper();

    virtual uint8_t read8(CpuAddress address) ;

    virtual void write8(CpuAddress address, uint8_t value);

protected:

    static const int PRG_RAM_SIZE;
    static const CpuAddress PRG_RAM_ADDR;
    static const int PRG_ROM_SIZE;
    static const CpuAddress PRG_ROM_ADDR;

    bool theUpper8KMirrored;

    uint8_t* thePrgRamRom;

    uint8_t* thePrgRom;
};

#endif // NROMMAPPER_H
