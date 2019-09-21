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

   virtual uint8_t read8(CpuAddress absAddr) override;

   virtual bool write8(CpuAddress absAddr, uint8_t val) override;

   virtual uint16_t read16(CpuAddress absAddr) override;

   virtual bool write16(CpuAddress absAddr, uint16_t val) override;

   // Configuration methods

   virtual bool isFullyConfigured() const override;

   virtual std::string getConfigTypeName() const;

   virtual std::vector<std::string> getIntConfigParams() const override;

   virtual std::vector<std::string> getStringConfigParams() const override;

   virtual void setIntConfigValue(std::string paramName, int value) override;
   virtual void setStringConfigValue(std::string paramName, std::string value) override;

   virtual void resetMemory() override;

   virtual bool specifiesStartAddress() const override;

   virtual CpuAddress getStartPcAddress() const;

   virtual bool configSelf() override;

protected:

   void loadRomIntoMemory();

   uint8_t* theData;

   std::string theRomFile;

   int theConfigFlags;

   bool theStartEmulationAddressSetFlag;

   CpuAddress theStartEmulationAddress;
};

#endif // ROMMEMORY_H
