#ifndef MEMORYCONTROLLER_H
#define MEMORYCONTROLLER_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include "Cpu6502Defines.h"

class MemoryDev;

/**
 * Controls what memory devices are at each particular set of memory addresses when a decoder
 * tries to read or write to them.
 *
 * If the processor has any custom bank switching operations that can change which devices are at
 * each memory location, then a child class of this class will have to implement those custom
 * features
 */
class MemoryController
{
public:
    MemoryController();

    std::vector<std::string> getDeviceNames();

    void addNewDevice(MemoryDev* device);

    void deleteDevice(MemoryDev* device);

    MemoryDev* getDevice(CpuAddress address);

protected:

    std::vector<MemoryDev*> theDevices;
};

#endif // MEMORYCONTROLLER_H
