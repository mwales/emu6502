#ifndef ROMMEMORY_H
#define ROMMEMORY_H


class RomMemory : public MemoryDev
{
public:
   RomMemory(std::string filepath, uint16_t address);

   ~RomMemory();

   void setName(std::string name);

   std::string getName();

   virtual uint8_t read8(uint16_t offset);

   virtual bool write8(uint16_t offset, uint8_t val);

   virtual uint16_t read16(uint16_t offset);

   virtual bool write16(uint16_t offset, uint16_t val);

   virtual uint16_t getAddress();

protected:

   uint16_t theAddress;

   uint16_t theSize;

   std::string theName;

   uint8_t* theData;
};

#endif // ROMMEMORY_H
