#include "MemoryFactory.h"
#include "EmulatorConfig.h"
#include "ConfigManager.h"
#include "MemoryController.h"
#include "Logger.h"
#include "EmulatorConfig.h"


#ifdef MEMORY_FACTORY_DEBUG
   #define MFACTORY_DEBUG   LOG_DEBUG
   #define MFACTORY_WARNING LOG_WARNING
#else
   #define MFACTORY_DEBUG   if(0) LOG_DEBUG
   #define MFACTORY_WARNING if(0) LOG_WARNING
#endif

// static instance pointer
MemoryFactory* MemoryFactory::theInstance = nullptr;

MemoryFactory* MemoryFactory::getInstance()
{
   if (theInstance == nullptr)
   {
      theInstance = new MemoryFactory();
      return theInstance;
   }
   else
   {
      return theInstance;
   }
}

void MemoryFactory::deleteInstance()
{
   if (theInstance != nullptr)
   {
      delete theInstance;
      theInstance = nullptr;
   }
}

std::vector<std::string> MemoryFactory::getMemoryTypes()
{
   /// @todo implement
   std::vector<std::string> retVal;
   return retVal;
}

MemoryDev* MemoryFactory::createMemoryDevice(std::string const & memoryType, 
                                             std::string const & instanceName)
{
   /// @todo implement
   return nullptr;
}

void MemoryFactory::registerMemoryDeviceType(std::string const & memoryType,
                                             MemoryDeviceConstructor mdc)
{
   /// @todo implement
   MFACTORY_DEBUG() << "Registering memory type: " << memoryType;
}


MemoryFactory::MemoryFactory()
{
   MFACTORY_DEBUG() << "Created a Memory Factory";
}

MemoryFactory::~MemoryFactory()
{
   MFACTORY_DEBUG() << "Destroying a Memory Factory";
}


void MemoryFactory::processSingleMemoryType(std::string typeName, MemoryDeviceConstructor mdc)
{
   //   ConfigManager* theConfig = ConfigManager::getInstance();
   
   //   // Process RAM configuration
   //   std::set<std::string> devInstanceNames;
   //   devInstanceNames = theConfig->getConfigTypeInstanceNames(typeName);
   //   for(auto const & singleInstance: devInstanceNames)
   //   {
   //      MemoryDev* currentDev = mdc(singleInstance);
   
   //      currentDev->setMemoryController(theMemoryController);
   
   //      if (currentDev->configSelf())
   //      {
   //         LOG_DEBUG() << "ConfigSelf success for " << typeName << "." << singleInstance;
   //         // currentDev->resetMemory();
   //         theMemoryController->addNewDevice(currentDev);
   //      }
   //      else
   //      {
   //         LOG_DEBUG() << "ConfigSelf failed for " << typeName << "." << singleInstance;
   //         delete currentDev;
   //      }
   //   }
}


