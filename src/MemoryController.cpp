#include "MemoryController.h"
#include "MemoryDev.h"
#include "Logger.h"

MemoryController::MemoryController()
{

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
