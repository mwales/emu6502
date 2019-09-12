#ifndef MEMORYFACTORY_H
#define MEMORYFACTORY_H

#include "Cpu6502Defines.h"
#include "MemoryDev.h"

class MemoryController;

/**
 * Class instantiates all the memory devices from the configuration data and adds them to the
 * memory controller.  The memory controller will take care of deleting the objects when it is
 * finished.
 */
class MemoryFactory
{
public:
   MemoryFactory(MemoryController* memController);

   void processConfigData();

   bool isStartAddessSet();

   CpuAddress getStartAddress();

protected:

   void processSingleMemoryType(std::string typeName, MemoryDeviceConstructor mdc);

   MemoryController* theMemoryController;

   bool theStartAddressSetFlag;

   CpuAddress theStartAddress;
};

#endif // MEMORYFACTORY_H
