#include "MemoryFactory.h"
#include "EmulatorConfig.h"
#include "ConfigManager.h"
#include "MemoryController.h"
#include "Logger.h"

// Memory devices
#include "RamMemory.h"
#include "RomMemory.h"
#include "MirrorMemory.h"
#include "RngDev.h"
#include "Easy6502JsDisplay.h"
#include "Easy6502JsInputDevice.h"
#include "NesRom.h"
#include "NesPpuDisplayDevice.h"

#ifdef MEMORY_FACTORY_DEBUG
   #define MFACTORY_DEBUG   LOG_DEBUG
   #define MFACTORY_WARNING LOG_WARNING
#else
   #define MFACTORY_DEBUG   if(0) LOG_DEBUG
   #define MFACTORY_WARNING if(0) LOG_WARNING
#endif

MemoryFactory::MemoryFactory(MemoryController* memController):
   theStartAddressSetFlag(false),
   theStartAddress(0)
{
   MFACTORY_DEBUG() << "Created a Memory Factory";
   theMemoryController = memController;
}

void MemoryFactory::processConfigData()
{
   // Process RAM configuration
   processSingleMemoryType(RamMemory::getTypeName(), RamMemory::getMDC());
   processSingleMemoryType(RomMemory::getTypeName(), RomMemory::getMDC());
   processSingleMemoryType(RngDev::getTypeName(), RngDev::getMDC());
   processSingleMemoryType(MirrorMemory::getTypeName(), MirrorMemory::getMDC());
   processSingleMemoryType(Easy6502JsDisplay::getTypeName(), Easy6502JsDisplay::getMDC());
   processSingleMemoryType(NesRom::getTypeName(), NesRom::getMDC());

}

void MemoryFactory::processSingleMemoryType(std::string typeName, MemoryDeviceConstructor mdc)
{
   ConfigManager* theConfig = ConfigManager::getInstance();

   // Process RAM configuration
   std::set<std::string> devInstanceNames;
   devInstanceNames = theConfig->getConfigTypeInstanceNames(typeName);
   for(auto const & singleInstance: devInstanceNames)
   {
      MemoryDev* currentDev = mdc(singleInstance);

      currentDev->setMemoryController(theMemoryController);

      if (currentDev->configSelf())
      {
         LOG_DEBUG() << "ConfigSelf success for " << typeName << "." << singleInstance;
         // currentDev->resetMemory();
         theMemoryController->addNewDevice(currentDev);
      }
      else
      {
         LOG_DEBUG() << "ConfigSelf failed for " << typeName << "." << singleInstance;
         delete currentDev;
      }
   }
}

bool MemoryFactory::isStartAddessSet()
{
   MFACTORY_DEBUG() << "MemoryFactory::isStartAddressSet() = "
                    << (theStartAddressSetFlag ? "True" : "False");

   return theStartAddressSetFlag;
}

CpuAddress MemoryFactory::getStartAddress()
{
   if (!theStartAddressSetFlag)
   {
      MFACTORY_WARNING() << "getStartAddress called, but none set";
      return 0;
   }

   MFACTORY_DEBUG() << "MemoryFactory::getStartAddress() = " << addressToString(theStartAddress);
   return theStartAddress;
}
