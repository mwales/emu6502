#ifndef RAMMEMORY_H
#define RAMMEMORY_H

#include "MemoryDev.h"
#include "EmulatorCommon.h"
#include <iostream>
#include <vector>
#include <cstdint>


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

   virtual std::string getConfigTypeName() const;

   virtual void resetMemory() override;

   virtual uint32_t getSaveStateLength() override;

   virtual bool saveState(uint8_t* buffer, uint32_t* bytesSaved) override;

   virtual bool loadState(uint8_t* buffer, uint32_t* bytesLoaded) override;

protected:

   int theConfigFlags;

   uint8_t* theData;
   
   CpuAddress theLoadDataOffset;
   
   std::string theLoadDataFile;
};

#endif // RAMMEMORY_H
