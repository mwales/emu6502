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
   theDevices.push_back(device);

   LOG_DEBUG() << "Memory Controller added device " << device->getName() << " at addr " << addressToString(device->getAddress());
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
   return nullptr;
}
