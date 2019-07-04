#ifndef RNGDEV_H
#define RNGDEV_H

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

};

#endif // RNGDEV_H
