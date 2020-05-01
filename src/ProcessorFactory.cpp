#include "ProcessorFactory.h"
#include "EmulatorConfig.h"
#include "Logger.h"
#include "EmulatorConfig.h"


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



ProcessorFactory::ProcessorFactory()
{
   PFACTORY_DEBUG() << "Created a Processor Factory";
}

ProcessorFactory::~ProcessorFactory()
{
   PFACTORY_DEBUG() << "Destroying a Processor Factory";
}





