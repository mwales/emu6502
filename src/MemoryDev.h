#ifndef MEMORYDEV_H
#define MEMORYDEV_H

#include<iostream>
#include<stdint.h>


class MemoryDev
{
public:
   std::string getName() = 0;

   virtual uint8_t read8(uint16_t offset) = 0;

   virtual bool write8(uint16_t offset, uint8_t val) = 0;

   virtual uint16_t read16(uint16_t offset) = 0;

   virtual bool write16(uint16_t offset, uint16_t val) = 0;

   virtual uint16_t getAddress() = 0;
};

#endif // MEMORYDEV_H
