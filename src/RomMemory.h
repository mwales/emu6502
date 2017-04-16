#ifndef ROMMEMORY_H
#define ROMMEMORY_H

#include "MemoryDev.h"

class RomMemory : public MemoryDev
{
public:
   RomMemory(std::string filepath, CpuAddress address);

   ~RomMemory();

   void setName(std::string name);



   virtual uint8_t read8(CpuAddress absAddr);

   virtual bool write8(CpuAddress absAddr, uint8_t val);

   virtual uint16_t read16(CpuAddress absAddr);

   virtual bool write16(CpuAddress absAddr, uint16_t val);



protected:

   uint8_t* theData;
};

#endif // ROMMEMORY_H
