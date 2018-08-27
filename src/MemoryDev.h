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

   std::string getName();

   virtual uint8_t read8(CpuAddress offset) = 0;

   virtual bool write8(CpuAddress offset, uint8_t val) = 0;

   virtual uint16_t read16(CpuAddress offset) = 0;

   virtual bool write16(CpuAddress offset, uint16_t val) = 0;

   virtual CpuAddress getAddress();

   virtual CpuAddress getSize();

   // Configuration Methods

   virtual bool isFullyConfigured();

   virtual std::vector<std::string> getIntConfigParams();

   virtual std::vector<std::string> getStringConfigParams();

   virtual void setIntConfigValue(std::string paramName, int value);
   virtual void setStringConfigValue(std::string paramName, std::string value);

   /**
    * This should be called after configuration complete.  Initializes the memory to starting
    * conditions
    */
   virtual void resetMemory() = 0;

   bool isAbsAddressValid(CpuAddress addr , bool haltOnError = true);

   MemoryRange getAddressRange();

   std::string getDebugString();

protected:   

   CpuAddress theAddress;

   CpuAddress theSize;

   std::string theName;

   MemoryController* theMemController;
};

#endif // MEMORYDEV_H
