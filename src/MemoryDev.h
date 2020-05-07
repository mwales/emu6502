#ifndef MEMORYDEV_H
#define MEMORYDEV_H

#include<iostream>
#include<stdint.h>
#include<vector>
#include<map>
#include "EmulatorCommon.h"
#include "SimpleMap.h"

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

   virtual std::string getConfigTypeName() const = 0;

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

   /// @todo Add ascii dump too
   std::string dump(bool asciiDump = false);

   virtual bool specifiesStartAddress() const;

   virtual CpuAddress getStartPcAddress() const;

   /// Configures self from ConfigManager.  Returns false if req'd config missing
   virtual bool configSelf();
   
   virtual std::string getMemoryConfigHelp();

protected:   

   CpuAddress theAddress;

   CpuAddress theSize;

   std::string theName;

   MemoryController* theMemController;

   SimpleMap<std::string, uint32_t*> theUint32ConfigParams;

   SimpleMap<std::string, uint16_t*> theUint16ConfigParams;

   SimpleMap<std::string, std::string*> theStrConfigParams;
};

#endif // MEMORYDEV_H
