#ifndef ROMMEMORY_H
#define ROMMEMORY_H

#include "MemoryDev.h"
#include <iostream>
#include <stdint.h>

class RomMemory : public MemoryDev
{
public:
   // Construction methods

   RomMemory(std::string name);
   static MemoryDeviceConstructor getMDC();
   static std::string getTypeName();

   virtual ~RomMemory();

   // Access methods

   virtual uint8_t read8(CpuAddress absAddr);

   virtual bool write8(CpuAddress absAddr, uint8_t val);

   virtual uint16_t read16(CpuAddress absAddr);

   virtual bool write16(CpuAddress absAddr, uint16_t val);

   // Configuration methods

   virtual bool isFullyConfigured();

   virtual std::vector<std::string> getIntConfigParams();

   virtual std::vector<std::string> getStringConfigParams();

   virtual void setIntConfigValue(std::string paramName, int value);
   virtual void setStringConfigValue(std::string paramName, std::string value);

   virtual void resetMemory();

protected:

   uint8_t* theData;

   std::string theRomFile;

   int theConfigFlags;
};

#endif // ROMMEMORY_H
