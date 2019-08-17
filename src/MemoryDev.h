#ifndef MEMORYDEV_H
#define MEMORYDEV_H

#include<iostream>
#include<stdint.h>
#include<vector>
#include "Cpu6502Defines.h"

class MemoryDev;
class MemoryController;

typedef MemoryDev* (*MemoryDeviceConstructor)(std::string instanceName);

/**
 * Defines the interface and common functionality for all memory based subsystems of the emulator
 */
class MemoryDev
{
public:
   MemoryDev(std::string const name);

   virtual ~MemoryDev();

   virtual void setMemoryController(MemoryController* mc);

   std::string getName() const;

   virtual uint8_t read8(CpuAddress absAddr) = 0;

   virtual bool write8(CpuAddress absAddr, uint8_t val) = 0;

   virtual uint16_t read16(CpuAddress absAddr) = 0;

   virtual bool write16(CpuAddress absAddr, uint16_t val) = 0;

   virtual CpuAddress getAddress() const;

   virtual CpuAddress getSize() const;

   // Configuration Methods

   virtual bool isFullyConfigured() const;

   virtual std::vector<std::string> getIntConfigParams() const;

   virtual std::vector<std::string> getStringConfigParams() const;

   virtual void setIntConfigValue(std::string paramName, int value);
   virtual void setStringConfigValue(std::string paramName, std::string value);

   /**
    * This should be called after configuration complete.  Initializes the memory to starting
    * conditions
    */
   virtual void resetMemory() = 0;

   bool isAbsAddressValid(CpuAddress addr , bool haltOnError = true) const;

   MemoryRange getAddressRange() const;

   std::string getDebugString() const;

   std::string dump();

   virtual bool specifiesStartAddress() const;

   virtual CpuAddress getStartPcAddress() const;

protected:   

   CpuAddress theAddress;

   CpuAddress theSize;

   std::string theName;

   MemoryController* theMemController;
};

#endif // MEMORYDEV_H
