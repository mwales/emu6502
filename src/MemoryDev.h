#ifndef MEMORYDEV_H
#define MEMORYDEV_H

#include<iostream>
#include<stdint.h>
#include "Cpu6502Defines.h"

/**
 * Defines the interface and common functionality for all memory based subsystems of the emulator
 */
class MemoryDev
{
public:
   std::string getName();

   virtual uint8_t read8(CpuAddress offset) = 0;

   virtual bool write8(CpuAddress offset, uint8_t val) = 0;

   virtual uint16_t read16(CpuAddress offset) = 0;

   virtual bool write16(CpuAddress offset, uint16_t val) = 0;

   virtual CpuAddress getAddress();

   virtual CpuAddress getSize();

   bool isAbsAddressValid(CpuAddress addr , bool haltOnError = true);

   MemoryRange getAddressRange();

   std::string getDebugString();

protected:   

   CpuAddress theAddress;

   CpuAddress theSize;

   std::string theName;
};

#endif // MEMORYDEV_H
