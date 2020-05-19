#include "ProcessorFactory.h"
#include "EmulatorConfig.h"
#include "Logger.h"
#include "EmulatorConfig.h"
#include "Debugger.h"
#include "ConfigManager.h"

#ifdef PROCESSOR_FACTORY_DEBUG
   #define PFACTORY_DEBUG   LOG_DEBUG
   #define PFACTORY_WARNING LOG_WARNING
#else
   #define PFACTORY_DEBUG   if(0) LOG_DEBUG
   #define PFACTORY_WARNING if(0) LOG_WARNING
#endif

// static instance pointer
ProcessorFactory* ProcessorFactory::theInstance = nullptr;

ProcessorFactory* ProcessorFactory::getInstance()
{
   if (theInstance == nullptr)
   {
      theInstance = new ProcessorFactory();
      return theInstance;
   }
   else
   {
      return theInstance;
   }
}

void ProcessorFactory::deleteInstance()
{
   if (theInstance != nullptr)
   {
      delete theInstance;
      theInstance = nullptr;
   }
}

std::vector<std::string> ProcessorFactory::getProcessorTypes()
{
   return theProcessorTypeList.getKeys();
}

Processor* ProcessorFactory::createProcessorInstance(std::string const & processorType, 
                                                     std::string const & instanceName)
{
   PFACTORY_DEBUG() << "Creating a new" << processorType << "(" << instanceName << ")";
   ProcessorConstructor pc = theProcessorTypeList.getValue(processorType);
   return pc(instanceName);
}

void ProcessorFactory::registerProcessorType(std::string const &  processorType,
                                             ProcessorConstructor mdc)
{
   PFACTORY_DEBUG() << "Registering processor type: " << processorType;
   theProcessorTypeList.add(processorType, mdc);
}



ProcessorFactory::ProcessorFactory():
   theCpu(nullptr)
{
   PFACTORY_DEBUG() << "Created a Processor Factory";
}

ProcessorFactory::~ProcessorFactory()
{
   PFACTORY_DEBUG() << "Destroying a Processor Factory";
}

Processor* ProcessorFactory::instantiateProcessor()
{
   PFACTORY_DEBUG() << "Instantiate Processor";
   ConfigManager* cfgMgr = ConfigManager::getInstance();
   std::set<std::string> typeList = cfgMgr->getConfigTypeNames();
   
   const std::string CONFIG_TYPE = "CPU";
   const std::string CONFIG_INSTANCE = "processor";
   const std::string CONFIG_MEMBER = "type";
   
   if (cfgMgr->isConfigPresent(CONFIG_TYPE, CONFIG_INSTANCE, CONFIG_MEMBER))
   {
      // Lets instantiate, config, and add memory device to the memory controller
      std::string cpuType = cfgMgr->getStringConfigValue(CONFIG_TYPE, CONFIG_INSTANCE, CONFIG_MEMBER);
      
      PFACTORY_DEBUG() << "Instantiating a processor of type" << cpuType;
      
      if (theCpu != nullptr)
      {
         PFACTORY_WARNING() << "The CPU has already been instantiated!";
         return theCpu;
      }
      
      theCpu = createProcessorInstance(cpuType, CONFIG_INSTANCE);
      return theCpu;
   }
   else
   {
      std::cerr << "No processor configuration found!" << std::endl;
      return nullptr;
   }
   
   
}



