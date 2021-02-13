#include <stdio.h>
#include "EmulatorCommon.h"
#include "MemoryController.h"
#include "MemoryDev.h"
#include "Logger.h"
#include "EmulatorConfig.h"
#include "Utils.h"


const char* DUMP_BYTE_COMMAND = "d8";
const char* DUMP_WORD_COMMAND = "d16";
const char* DUMP_DWORD_COMMAND = "d32";

MemoryController::MemoryController():
   theLastDumpAddress(0),
   theLittleEndianFlag(true)
{

}

MemoryController::~MemoryController()
{
#ifdef DUMP_MEMORY
   debugDumpMemoryController(true);
#endif

   while(theDevices.size() > 0)
   {
      MemoryDev* theCurrentDev = theDevices.back();
      theDevices.pop_back();

      LOG_DEBUG() << "Deleting MemoryDev:" << theCurrentDev->getDebugString();
      delete theCurrentDev;
   }
}

std::vector<std::string> MemoryController::getDeviceNames()
{
   std::vector<std::string> retData;
   for(auto curDevice = theDevices.begin(); curDevice != theDevices.end(); curDevice++)
   {
      retData.push_back((*curDevice)->getName());
   }

   return retData;
}

void MemoryController::addNewDevice(MemoryDev* device)
{
   // Make sure new device does not overlap the memory of existing memory devices
   for(auto existingDevice = theDevices.begin(); existingDevice != theDevices.end(); existingDevice++)
   {
      if (doRangesOverlap((*existingDevice)->getAddressRange(), device->getAddressRange()))
      {
         LOG_WARNING() << "Adding a new memory device failed due to overlapping ranges!";
         LOG_WARNING() << device->getDebugString() << " overlaps with " << (*existingDevice)->getDebugString();
         return;
      }
   }


   for(auto existingDevice = theDevices.begin(); existingDevice != theDevices.end(); existingDevice++)
   {
      if (device->getAddress() < (*existingDevice)->getAddress())
      {
         theDevices.insert(existingDevice, device);
         LOG_DEBUG() << "Memory Controller added device " << device->getDebugString();
         return;
      }
   }

   theDevices.push_back(device);
   LOG_DEBUG() << "Memory Controller added device " << device->getDebugString();
}

void MemoryController::deleteDevice(MemoryDev* device)
{
   for(auto curDevice = theDevices.begin(); curDevice != theDevices.end(); curDevice++)
   {
      if (*curDevice == device)
      {
         theDevices.erase(curDevice);
         return;
      }
   }

   LOG_WARNING() << "Can't find device " << device->getName() << " in list of devices to delete it!";
}

MemoryDev* MemoryController::getDevice(CpuAddress address)
{
   for(auto curDevice = theDevices.begin(); curDevice != theDevices.end(); curDevice++)
   {
      if ( (*curDevice)->getAddress() > address)
      {
         // Address probing before device, can't be this device
         continue;
      }

      CpuAddress offsetWithinDevice = address - (*curDevice)->getAddress();

      if (offsetWithinDevice < (*curDevice)->getSize())
      {
         return (*curDevice);
      }

   }

   // No matching device found
   LOG_WARNING() << "Memory Controller has no device for address " << Utils::toHex16(address);
   debugDumpMemoryController();
   return nullptr;
}

MemoryDev* MemoryController::getDevice(std::string instanceName)
{
   for(auto curDevice: theDevices)
   {
      if (instanceName == curDevice->getName())
      {
         return curDevice;
      }
   }

   // No matching device found
   LOG_WARNING() << "Memory Controller has no device for instance name " << instanceName;
   return nullptr;
}

std::vector<MemoryRange> MemoryController::getOrderedRangeList()
{
   std::vector<MemoryRange> retVal;

   for(auto curDevice = theDevices.begin(); curDevice != theDevices.end(); curDevice++)
   {
      retVal.push_back( (*curDevice)->getAddressRange());
   }

   return retVal;
}

void MemoryController::resetAll()
{
   LOG_DEBUG() << "Reseting all memory devices";

   for(auto md: theDevices)
   {
      md->resetMemory();
   }
}

std::vector<MemoryDev*> MemoryController::getAllDevices()
{
   return theDevices;
}



bool MemoryController::doRangesOverlap(MemoryRange dev1, MemoryRange dev2)
{
   // [ dev1 ]
   //     [   dev2  ]
   if ( (dev1.second >= dev2.first) && (dev2.second >= dev1.first) )
   {
      return true;
   }


   if ( (dev2.second >= dev1.first) && (dev1.second >= dev2.first) )
   {
      return true;
   }

   return false;
}

bool MemoryController::doRangesMerge(MemoryRange dev1, MemoryRange dev2)
{
   if ( (dev1.second + 1) == dev2.first)
   {
      return true;
   }
   if ( (dev2.second + 1) == dev1.first)
   {
      return true;
   }

   return false;
}

MemoryRange MemoryController::mergeRanges(MemoryRange dev1, MemoryRange dev2)
{
   MemoryRange retVal;
   if ( (dev1.second + 1) == dev2.first)
   {
      retVal.first = dev1.first;
      retVal.second = dev2.second;
      return retVal;
   }
   if ( (dev2.second + 1) == dev1.first)
   {
      retVal.first = dev2.first;
      retVal.second = dev1.second;
      return retVal;
   }

   LOG_FATAL() << "mergeRanges called with non-mergable ranges!";
   retVal.first = 0;
   retVal.second = 0;
   return retVal;

}

void MemoryController::debugDumpMemoryController(bool dumpContents)
{
   FILE* dumpFile = NULL;
   if (dumpContents)
   {
      // Open the memory.dump file
      LOG_DEBUG() << "Opening dump.txt for memory dump";
      dumpFile = fopen("dump.txt", "w+");

      if (dumpFile == NULL)
      {
         LOG_WARNING() << "Can't dump memory contents to file.  Unable to open file";
         LOG_WARNING() << strerror(errno);
         dumpContents = false;
      }
   }

   for(auto * singleDev : theDevices)
   {
      LOG_DEBUG() << singleDev->getDebugString();

      if (dumpContents)
      {
         fprintf(dumpFile, "%s", singleDev->dump(true).c_str());
      }
   }

   if (dumpContents)
   {
      // Close the memory.dump file
      LOG_DEBUG() << "Closing dump.txt memory dump file";
      fclose(dumpFile);
   }
}

void MemoryController::setLittleEndianMode(bool le)
{
    theLittleEndianFlag = le;
}

bool MemoryController::read8(CpuAddress addr, uint8_t* val)
{
   MemoryDev* mdev = getDevice(addr);
   if (mdev == nullptr)
   {
      LOG_WARNING() << "Invalid read8 @ " << addressToString(addr);
      return false;
   }
   
   *val = mdev->read8(addr);
   return true;
}

bool MemoryController::read16(CpuAddress addr, uint16_t* val)
{
   uint16_t retVal = 0;
   int numBytesShift = (theLittleEndianFlag ? 8 : 0);
   
   for(int byteNum = 0; byteNum < 2; byteNum++)
   {
      MemoryDev* mdev = getDevice(addr + byteNum);
      if (mdev == nullptr)
      {
         LOG_DEBUG() << "Error reading memory for read16 @ " << addressToString(addr);
         return false;
      }
      
      uint16_t temp = mdev->read8(addr + byteNum);
      temp = temp << numBytesShift;
      retVal |= temp;
      
      if (theLittleEndianFlag)
      {
         numBytesShift -= 8;
      }
      else
      {
         numBytesShift += 8;
      }
   }
   
   *val = retVal;
   return true;
}

bool MemoryController::read32(CpuAddress addr, uint32_t* val)
{
   uint32_t retVal = 0;
   int numBytesShift = (theLittleEndianFlag ? 24 : 0);
   
   for(int byteNum = 0; byteNum < 4; byteNum++)
   {
      MemoryDev* mdev = getDevice(addr + byteNum);
      if (mdev == nullptr)
      {
         LOG_DEBUG() << "Error reading memory for read32 @ " << addressToString(addr);
         return false;
      }
      
      uint32_t temp = mdev->read8(addr + byteNum);
      temp = temp << numBytesShift;
      retVal |= temp;
      
      if (theLittleEndianFlag)
      {
         numBytesShift -= 8;
      }
      else
      {
         numBytesShift += 8;
      }
   }
   
   *val = retVal;
   return true;
}

bool MemoryController::write8(CpuAddress addr, uint8_t val)
{
   MemoryDev* mdev = getDevice(addr);
   if (mdev == nullptr)
   {
      LOG_WARNING() << "Invalid write8 @ " << addressToString(addr);
      return false;
   }
   
   mdev->write8(addr, val);
   return true;
}

bool MemoryController::write16(CpuAddress addr, uint16_t val)
{
   int numBytesShift = (theLittleEndianFlag ? 8 : 0);
   
   for(int byteNum = 0; byteNum < 2; byteNum++)
   {
      MemoryDev* mdev = getDevice(addr + byteNum);
      if (mdev == nullptr)
      {
         LOG_DEBUG() << "Error reading memory for write16 @ " << addressToString(addr);
         return false;
      }
      
      uint8_t temp = val >> numBytesShift & 0xff;
      mdev->write8(addr + byteNum, temp);
            
      if (theLittleEndianFlag)
      {
         numBytesShift -= 8;
      }
      else
      {
         numBytesShift += 8;
      }
   }
   
   return true;
}

bool MemoryController::write32(CpuAddress addr, uint32_t val)
{
   int numBytesShift = (theLittleEndianFlag ? 24 : 0);
   
   for(int byteNum = 0; byteNum < 4; byteNum++)
   {
      MemoryDev* mdev = getDevice(addr + byteNum);
      if (mdev == nullptr)
      {
         LOG_DEBUG() << "Error reading memory for write32 @ " << addressToString(addr);
         return false;
      }
      
      uint8_t temp = val >> numBytesShift & 0xff;
      mdev->write8(addr + byteNum, temp);
            
      if (theLittleEndianFlag)
      {
         numBytesShift -= 8;
      }
      else
      {
         numBytesShift += 8;
      }
   }
   
   return true;
}

DECLARE_DEBUGGER_CALLBACK(MemoryController, dump8CommandHandler);
DECLARE_DEBUGGER_CALLBACK(MemoryController, dump16CommandHandler);
DECLARE_DEBUGGER_CALLBACK(MemoryController, dump32CommandHandler);
DECLARE_DEBUGGER_CALLBACK(MemoryController, memdevsCommandHandler);

void MemoryController::registerDebuggerCommands(Debugger* dbgr)
{
   dbgr->registerNewCommandHandler(DUMP_BYTE_COMMAND, "Dump memory by 8-bit byte",
                                   g_dump8CommandHandler,
                                   this);
   dbgr->registerNewCommandHandler(DUMP_WORD_COMMAND, "Dump memory by 16-bit word",
                                   g_dump16CommandHandler,
                                   this);
   dbgr->registerNewCommandHandler(DUMP_DWORD_COMMAND, "Dump memory by 32-bit dword",
                                   g_dump32CommandHandler,
                                   this);
   dbgr->registerNewCommandHandler("memdevs", "List Memory Devices",
                                   g_memdevsCommandHandler,
                                   this);
   
}



void MemoryController::printDebuggerUsage(std::string commandName)
{
   std::cout << commandName << " help" << std::endl;
   std::cout << "  " << commandName << ": Dumps 40 bytes after the last dump call" << std::endl;;
   std::cout << "  " << commandName << " [address]: Dumps 40 bytes after the specified address" << std::endl;
   std::cout << "  " << commandName << " [address] [numbytes]: Dumps numbytes after the specified address" << std::endl;
}

void MemoryController::dump8CommandHandler(std::vector<std::string> const & args)
{
   if ( (args.size() >= 1) && (args[0] == "help") )
   {
      printDebuggerUsage(DUMP_BYTE_COMMAND);
      return;
   }
   
   CpuAddress addrToDump = theLastDumpAddress;
   uint32_t numDump = 16 * 8;
   if ( args.size() >= 1)
   {
      if(!stringToAddress(args[0], &addrToDump))
      {
         std::cout << "Address " << args[0] << " invalid" << std::endl;
         return;
      }
   }
   
   if ( args.size() >= 2)
   {
      bool success;
      uint32_t nb = Utils::parseUInt32(args[1], &success);
      if(success)
      {
         numDump = nb;
      }
      else
      {
         std::cout << "Num bytes to dump " << args[1] << " invalid" << std::endl;
         return;
      }
   }
   
   CpuAddress currentDumpAddr = addrToDump & (~0xf);
   CpuAddress endDumpAddr = addrToDump + numDump;
   uint32_t numBytesDumped = 0;
   while(numBytesDumped < numDump)
   {
      // Dump single line
      std::cout << addressToString(currentDumpAddr) << ":  ";
      for(int i = 0; i < 16; i++)
      {
         if ( (currentDumpAddr >= addrToDump) && (currentDumpAddr < endDumpAddr) )
         {
            uint8_t value;
            bool readSuccess = read8(currentDumpAddr, &value);
            if (readSuccess)
            {
               std::cout << Utils::toHex8(value, false) << " ";
            }
            else
            {
               std::cout << "-- ";
            }
            numBytesDumped++;
         }
         else
         {
            std::cout << "   ";
         }
         
         currentDumpAddr++;
      }
      
      std::cout << std::endl;
   }
   
   theLastDumpAddress = addrToDump + numDump;
   
}

void MemoryController::dump16CommandHandler(std::vector<std::string> const & args)
{
   if ( (args.size() >= 1) && (args[0] == "help") )
   {
      printDebuggerUsage(DUMP_WORD_COMMAND);
      return;
   }
   
   CpuAddress addrToDump = theLastDumpAddress;
   uint32_t numDump = 16 * 8;
   if ( args.size() >= 1)
   {
      if(!stringToAddress(args[0], &addrToDump))
      {
         std::cout << "Address " << args[0] << " invalid" << std::endl;
         return;
      }
   }
   
   // Put on a word boundary
   theLastDumpAddress &= ~0x1;
   
   if ( args.size() >= 2)
   {
      bool success;
      uint32_t nb = Utils::parseUInt32(args[1], &success);
      if(success)
      {
         numDump = nb;
      }
      else
      {
         std::cout << "Num bytes to dump " << args[1] << " invalid" << std::endl;
         return;
      }
   }
   
   CpuAddress currentDumpAddr = addrToDump & (~0xf);
   CpuAddress endDumpAddr = addrToDump + numDump;
   uint32_t numBytesDumped = 0;
   while(numBytesDumped < numDump)
   {
      // Dump single line
      std::cout << addressToString(currentDumpAddr) << ":  ";
      for(int i = 0; i < 16; i += 2)
      {
         if ( (currentDumpAddr >= addrToDump) && (currentDumpAddr < endDumpAddr) )
         {
            uint16_t value;
            bool readSuccess = read16(currentDumpAddr, &value);
            if (readSuccess)
            {
               std::cout << Utils::toHex16(value, false) << " ";
            }
            else
            {
               std::cout << "---- ";
            }
            numBytesDumped += 2;
         }
         else
         {
            std::cout << "     ";
         }
         
         currentDumpAddr += 2;
      }
      
      std::cout << std::endl;
   }
   
   theLastDumpAddress = addrToDump + numDump;
}

void MemoryController::dump32CommandHandler(std::vector<std::string> const & args)
{
   if ( (args.size() >= 1) && (args[0] == "help") )
   {
      printDebuggerUsage(DUMP_DWORD_COMMAND);
      return;
   }
   
   CpuAddress addrToDump = theLastDumpAddress;
   uint32_t numDump = 16 * 8;
   if ( args.size() >= 1)
   {
      if(!stringToAddress(args[0], &addrToDump))
      {
         std::cout << "Address " << args[0] << " invalid" << std::endl;
         return;
      }
   }
   
   // Put on a dword boundary
   theLastDumpAddress &= ~0x3;
   
   if ( args.size() >= 2)
   {
      bool success;
      uint32_t nb = Utils::parseUInt32(args[1], &success);
      if(success)
      {
         numDump = nb;
      }
      else
      {
         std::cout << "Num bytes to dump " << args[1] << " invalid" << std::endl;
         return;
      }
   }
   
   CpuAddress currentDumpAddr = addrToDump & (~0xf);
   CpuAddress endDumpAddr = addrToDump + numDump;
   uint32_t numBytesDumped = 0;
   while(numBytesDumped < numDump)
   {
      // Dump single line
      std::cout << addressToString(currentDumpAddr) << ":  ";
      for(int i = 0; i < 16; i += 4)
      {
         if ( (currentDumpAddr >= addrToDump) && (currentDumpAddr < endDumpAddr) )
         {
            uint32_t value;
            bool readSuccess = read32(currentDumpAddr, &value);
            if (readSuccess)
            {
               std::cout << Utils::toHex32(value, false) << " ";
            }
            else
            {
               std::cout << "-------- ";
            }
            numBytesDumped += 4;
         }
         else
         {
            std::cout << "         ";
         }
         
         currentDumpAddr += 4;
      }
      
      std::cout << std::endl;
   }
   
   theLastDumpAddress = addrToDump + numDump;
}


void MemoryController::memdevsCommandHandler(std::vector<std::string> const & args)
{
   for(auto curDevName: getDeviceNames())
   {
      std::cout << "Device: " << curDevName << std::endl;
      MemoryDev* md = getDevice(curDevName);
      if (md == nullptr)
      {
         std::cout << "  internal error: device nullptr" << std::endl;
      }
      else
      {
         std::cout << "  Address: " << addressToString(md->getAddress()) << " ("
                   << md->getSize() << " bytes)" << std::endl;
      }
   }
}


