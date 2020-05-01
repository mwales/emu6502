#ifndef PROCESSORFACTORY_H
#define PROCESSORFACTORY_H


#include "EmulatorCommon.h"
#include <string>
#include <vector>
#include <utility> // std::pair
#include "Processor.h"
#include "SimpleMap.h"


// The processor factory will probably get instantiated by C++ static constructors,
// so it must stay a tradition singleton

/**
 * Class handles registration of the different processor types, and will instantiate
 * the processor the user configured.
 */
class ProcessorFactory
{
public:
   static ProcessorFactory* getInstance();
   
   static void deleteInstance();
   
   std::vector<std::string> getProcessorTypes();
   
   Processor* createProcessorInstance(std::string const & processorType,
                                      std::string const & instanceName);
   
   void registerProcessorType(std::string const & memoryType, ProcessorConstructor mdc);
   
protected:
   
   // Singleton protections
   ProcessorFactory();
   ~ProcessorFactory();
   
   void registerProcessorType(std::string typeName, ProcessorConstructor mdc);
   
   /// Always add and remove from both of these vectors
   SimpleMap<std::string, ProcessorConstructor> theProcessorTypeList;
   
   static ProcessorFactory* theInstance;
};

#endif // PROCESSORFACTORY_H
