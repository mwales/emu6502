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

   virtual uint8_t read8(CpuAddress absAddr) override;

   virtual bool write8(CpuAddress absAddr, uint8_t val) override;

   virtual uint16_t read16(CpuAddress absAddr) override;

   virtual bool write16(CpuAddress absAddr, uint16_t val) override;

   // Configuration methods

   virtual bool isFullyConfigured() const override;

   virtual std::vector<std::string> getIntConfigParams() const override;

   virtual std::vector<std::string> getStringConfigParams() const override;

   virtual void setIntConfigValue(std::string paramName, int value) override;
   virtual void setStringConfigValue(std::string paramName, std::string value) override;

   virtual void resetMemory() override;

protected:

   int theConfigFlags;

   uint8_t* theData;
};

#endif // RAMMEMORY_H
