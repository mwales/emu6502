#ifndef MEMORYCONTROLLER_H
#define MEMORYCONTROLLER_H

#include <vector>
#include <stdint.h>
#include <iostream>


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

    ~MemoryController();

    std::vector<std::string> getDeviceNames();

    void addNewDevice(MemoryDev* device);

    void deleteDevice(MemoryDev* device);

    MemoryDev* getDevice(CpuAddress address);

    /**
     * Gets a list of all the valid memory ranges available to the processor
     */
    std::vector<MemoryRange> getOrderedRangeList();

    void resetAll();

    std::vector<MemoryDev*> getAllDevices();



protected:
    /**
     * Do two memory ranges overlap each other for 1 or more addresses
     */
    bool doRangesOverlap(MemoryRange dev1, MemoryRange dev2);

    /**
     * Do 2 memory ranges directly neighbor each other (with no invalid addresses inbetween the two
     * ranges, and no overlapping addresses)
     */
    bool doRangesMerge(MemoryRange dev1, MemoryRange dev2);

    /**
     * Determines what memory range could represent both ranges togehter.
     *
     * @note Only call if doRangesMerge returns true!
     */
    MemoryRange mergeRanges(MemoryRange dev1, MemoryRange dev2);

    void debugDumpMemoryController(bool dumpContents = false);

    std::vector<MemoryDev*> theDevices;
};

#endif // MEMORYCONTROLLER_H
