#ifndef MEMORYDEV_H
#define MEMORYDEV_H

#include<iostream>
#include<stdint.h>


class MemoryDev
{
public:
   MemoryDev(uint16_t address);

   std::string getName();

   virtual uint8_t read8(uint16_t offset) = 0;

   virtual bool write8(uint16_t offset, uint8_t val) = 0;

   virtual uint16_t read16(uint16_t offset) = 0;

   virtual bool write16(uint16_t offset, uint16_t val) = 0;

   virtual uint16_t getAddress();

protected:

   bool isAbsAddressValid(uint16_t addr);

   uint16_t theAddress;

   uint16_t theSize;

   std::string theName;
};

#endif // MEMORYDEV_H
