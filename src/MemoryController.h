#ifndef MEMORYCONTROLLER_H
#define MEMORYCONTROLLER_H

#include <vector>
#include <cstdint>
#include <iostream>
#include "EmulatorCommon.h"
#include "Debugger.h"


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
   
   void setLittleEndianMode(bool le);
   
   bool read8(CpuAddress addr, uint8_t* val);
   
   bool read16(CpuAddress addr, uint16_t* val);
   
   bool read32(CpuAddress addr, uint32_t* val);
   
   bool write8(CpuAddress addr, uint8_t val);
   
   bool write16(CpuAddress addr, uint16_t val);
   
   bool write32(CpuAddress addr, uint32_t val);
   
   void registerDebuggerCommands(Debugger* dbgr);
   
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
   
   CpuAddress theLastDumpAddress;
   
   void printDebuggerUsage(std::string commandName);
   
   static void dump8CommandHandlerStatic(std::vector<std::string> const & args, 
                                         void* context);
   static void dump16CommandHandlerStatic(std::vector<std::string> const & args, 
                                          void* context);
   static void dump32CommandHandlerStatic(std::vector<std::string> const & args, 
                                          void* context);
   void dump8CommandHandler(std::vector<std::string> const & args);
   void dump16CommandHandler(std::vector<std::string> const & args);
   void dump32CommandHandler(std::vector<std::string> const & args);
   
   
   
   void debugDumpMemoryController(bool dumpContents = false);
   
   std::vector<MemoryDev*> theDevices;
   
   bool theLittleEndianFlag;
};

#endif // MEMORYCONTROLLER_H
