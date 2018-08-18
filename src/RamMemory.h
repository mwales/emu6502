#ifndef RAMMEMORY_H
#define RAMMEMORY_H

#include "MemoryDev.h"
#include <iostream>

/**
 * A memory device that is initialized to all zeros.  The emulator can read and
 * write from it.
 */
class RamMemory : public MemoryDev
{
public:
   // Construction methods

   RamMemory(std::string name);

   static MemoryDeviceConstructor getMDC();
   static std::string getTypeName();

   virtual ~RamMemory();

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

   int theConfigFlags;

   uint8_t* theData;
};

#endif // RAMMEMORY_H
