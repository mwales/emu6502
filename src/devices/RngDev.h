#ifndef RNGDEV_H
#define RNGDEV_H

#include "EmulatorCommon.h"
#include "MemoryDev.h"
#include <iostream>

/**
 * A memory device that returns random data when read.  Doesn't do anything when
 */
class RngDev : public MemoryDev
{
public:
   // Construction methods

   RngDev(std::string name);

   static MemoryDeviceConstructor getMDC();
   static std::string getTypeName();

   virtual ~RngDev();

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

protected:

   int theConfigFlags;

};

#endif // RNGDEV_H
