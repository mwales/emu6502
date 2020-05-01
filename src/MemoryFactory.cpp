#include "MemoryFactory.h"
#include "EmulatorConfig.h"
#include "ConfigManager.h"
#include "MemoryController.h"
#include "Logger.h"


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
   return theMemoryTypes.getKeys();
}

MemoryDev* MemoryFactory::createMemoryDevice(std::string const & memoryType, 
                                             std::string const & instanceName)
{
   MFACTORY_DEBUG() << "Creating a new" << memoryType << "(" << instanceName << ")";
   MemoryDeviceConstructor mdc = theMemoryTypes.getValue(memoryType);
   return mdc(instanceName);
}

void MemoryFactory::registerMemoryDeviceType(std::string const & memoryType,
                                             MemoryDeviceConstructor mdc)
{
   MFACTORY_DEBUG() << "Registering memory type: " << memoryType;
   theMemoryTypes.add(memoryType, mdc);
}


MemoryFactory::MemoryFactory()
{
   MFACTORY_DEBUG() << "Created a Memory Factory";
}

MemoryFactory::~MemoryFactory()
{
   MFACTORY_DEBUG() << "Destroying a Memory Factory";
}



