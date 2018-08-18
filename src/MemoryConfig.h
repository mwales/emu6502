#ifndef MEMORYCONFIG_H
#define MEMORYCONFIG_H

#include<iostream>
#include<map>

#include "MemoryController.h"
#include "MemoryDev.h"
#include "Cpu6502Defines.h"

#include "cjson/cJSON.h"

/**
 * Parses JSON configuration data and creates memory devices based on the configuration data
 */
class MemoryConfig
{
public:
   /**
     * Initializes list of all the memory types
     */
    MemoryConfig(MemoryController* mc);

    /**
     * Parses the configuration JSON text
     * @param configJson Text of configuration
     * @return True if JSON parsed without failure
     */
    bool parseConfig(std::string configJson);

    /**
     * Registers a function that this class can call to create a MemoryDev object
     *
     * @param deviceName The name given to the memory device
     * @param mdc Constructor to call
     */
    void registerMemoryDevice(std::string deviceName, MemoryDeviceConstructor mdc);

    std::string getConfigurationName();

    CpuAddress getStartAddress();

    bool isStartAddressSet();


protected:

    void parseDeviceArray(cJSON* j);

    void parseSingleDevice(cJSON* j);

    MemoryController* theMemoryController;

    std::string theConfigurationName;

    std::map<std::string, MemoryDeviceConstructor > theDevConstructorList;

    bool theStartAddressSet;
    CpuAddress theStartAddress;

};

#endif // MEMORYCONFIG_H
