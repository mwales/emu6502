#ifndef MEMORYFACTORY_H
#define MEMORYFACTORY_H


#include "EmulatorCommon.h"
#include <string>
#include <vector>
#include <utility> // std::pair
#include "MemoryDev.h"
#include "SimpleMap.h"


class MemoryController;

// The memory factory will probably get instantiated by C++ static constructors,
// so it must stay a tradition singleton

/**
 * Class instantiates all the memory devices from the configuration data and adds them to the
 * memory controller.  The memory controller will take care of deleting the objects when it is
 * finished.
 */
class MemoryFactory
{
public:
   static MemoryFactory* getInstance();
   
   static void deleteInstance();
   
   std::vector<std::string> getMemoryTypes();
   
   MemoryDev* createMemoryDevice(std::string const & memoryType,
                                 std::string const & instanceName);
   
   void registerMemoryDeviceType(std::string const & memoryType, MemoryDeviceConstructor mdc);
   
protected:
   
   // Singleton protections
   MemoryFactory();
   ~MemoryFactory();
   
   /// Always add and remove from both of these vectors
   SimpleMap<std::string, MemoryDeviceConstructor> theMemoryTypes;
   
   static MemoryFactory* theInstance;
};

#endif // MEMORYFACTORY_H
