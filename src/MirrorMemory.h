#ifndef MIRRORMEMORY_H
#define MIRRORMEMORY_H

#include "MemoryDev.h"
#include <iostream>

/**
 * A memory device that is a mirror of other memory devices.
 */
class MirrorMemory : public MemoryDev
{
public:
   // Construction methods

   MirrorMemory(std::string name);

   static MemoryDeviceConstructor getMDC();
   static std::string getTypeName();

   virtual ~MirrorMemory();

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

   CpuAddress theSizeOfMemoryMirrored;

   CpuAddress theAddrOfMemoryMirrored;

   std::string theMemoryMirroredName;

   MemoryDev* theRealMemDev;
};

#endif // MIRRORMEMORY_H
