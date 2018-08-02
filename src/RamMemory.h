#ifndef RAMMEMORY_H
#define RAMMEMORY_H

#include "MemoryDev.h"

/**
 * A memory device that is initialized to all zeros.  The emulator Can read and
 * write from it.
 */
class RamMemory : public MemoryDev
{
public:
   RamMemory(CpuAddress address, uint16_t size);

   ~RamMemory();

   void setName(std::string name);

   virtual uint8_t read8(CpuAddress absAddr);

   virtual bool write8(CpuAddress absAddr, uint8_t val);

   virtual uint16_t read16(CpuAddress absAddr);

   virtual bool write16(CpuAddress absAddr, uint16_t val);

protected:

   uint8_t* theData;
};

#endif // RAMMEMORY_H
