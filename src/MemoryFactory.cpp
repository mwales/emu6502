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

void MemoryFactory::printMemoryConfigHelp()
{
   // Construct an instance of each type  of memory and see what configuration it wants
   for(auto curMemType: theMemoryTypes.getKeys())
   {
      MemoryDev* curDev = theMemoryTypes.getValue(curMemType)("dummy");
      std::cout << curDev->getMemoryConfigHelp();
      delete curDev;
   }
}


MemoryFactory::MemoryFactory()
{
   MFACTORY_DEBUG() << "Created a Memory Factory";
}

MemoryFactory::~MemoryFactory()
{
   MFACTORY_DEBUG() << "Destroying a Memory Factory";
}

bool MemoryFactory::instantiateMemoryDevices(MemoryController* mc)
{
   MFACTORY_DEBUG() << "Instantiate Memory Devices";
   ConfigManager* cfgMgr = ConfigManager::getInstance();
   std::set<std::string> typeList = cfgMgr->getConfigTypeNames();
   for(auto curType: typeList)
   {
      if (theMemoryTypes.contains(curType))
      {
         // Lets instantiate, config, and add memory device to the memory controller
         MFACTORY_DEBUG() << "Creating all MemoryDev instances of type" << curType;
         
         std::set<std::string> instanceNames = cfgMgr->getConfigTypeInstanceNames(curType);
         for(auto curInstName: instanceNames)
         {
            MemoryDev* md = theMemoryTypes.getValue(curType)(curInstName);
            
            if (md->configSelf())
            {
               MFACTORY_DEBUG() << "Successfully created and configured" << curType << "."
                                << curInstName;
               md->setMemoryController(mc);
               mc->addNewDevice(md);
            }
            else
            {
               MFACTORY_WARNING() << "Failed to create and configure" << curType << "."
                                << curInstName;
               delete md;
            }
            
         }
      }
   }
   
}

