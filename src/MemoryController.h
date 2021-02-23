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
   
   MemoryDev* getDevice(std::string instanceName);
   
   /**
     * Gets a list of all the valid memory ranges available to the processor
     */
   std::vector<MemoryRange> getOrderedRangeList();
   
   void resetAll();
   
   std::vector<MemoryDev*> getAllDevices();

   std::vector<MemoryDev*> getAllDevicesInNameOrder();
   
   void setLittleEndianMode(bool le);
   
   bool read8(CpuAddress addr, uint8_t* val);
   
   bool read16(CpuAddress addr, uint16_t* val);
   
   bool read32(CpuAddress addr, uint32_t* val);
   
   bool write8(CpuAddress addr, uint8_t val);
   
   bool write16(CpuAddress addr, uint16_t val);
   
   bool write32(CpuAddress addr, uint32_t val);
   
   void registerDebuggerCommands(Debugger* dbgr);

   // Debugger command handlers
   void dump8CommandHandler(std::vector<std::string> const & args);
   void dump16CommandHandler(std::vector<std::string> const & args);
   void dump32CommandHandler(std::vector<std::string> const & args);
   void memdevsCommandHandler(std::vector<std::string> const & args);

   /**
    * @return Number of bytes required to save device state
    */
   uint32_t getSaveStateLength();

   /**
    * Saves the state of the device into the buffer provided
    *
    * @param buffer Where to save the data
    * @param[out] bytesSaved How many bytes were saved.  0 if the device doesn't save state.
    * @return False on save error, true if successful or no state to save.
    */
   bool saveState(uint8_t* buffer, uint32_t* bytesSaved);

   /**
    * Loads state of the device from a file
    *
    * @param buffer Where to load the data from
    * @param bytesLoaded[out] How many bytes were loaded from
    * @return False on load error, true if successful or no state to save
    */
   bool loadState(uint8_t* buffer, uint32_t* bytesLoaded);
   
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
   
   
   void debugDumpMemoryController(bool dumpContents = false);
   
   std::vector<MemoryDev*> theDevices;
   
   bool theLittleEndianFlag;
};

#endif // MEMORYCONTROLLER_H
