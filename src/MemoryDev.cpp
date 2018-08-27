#include"MemoryDev.h"
#include "Logger.h"
#include "Utils.h"

MemoryDev::MemoryDev(std::string name):
   theAddress(0),
   theSize(0),
   theName(name),
   theMemController(nullptr)
{
   // Purposely blank
}

MemoryDev::~MemoryDev()
{
   // Purposely blank
}

void MemoryDev::setMemoryController(MemoryController* mc)
{
    theMemController = mc;
}

std::string MemoryDev::getName()
{
   return theName;
}

CpuAddress MemoryDev::getAddress()
{
   return theAddress;
}

CpuAddress MemoryDev::getSize()
{
   return theSize;
}

bool MemoryDev::isAbsAddressValid(CpuAddress addr, bool haltOnError)
{
   if (addr < theAddress)
   {
      LOG_FATAL() << "Memory (" << theName << ") - Address " << addressToString(addr) << " is below the start address of ROM " << theName;
      return false;
   }

   if (addr > (theAddress + theSize))
   {
      LOG_FATAL() << "Memory (" << theName << ") - Address " << addressToString(addr) << " is after the end address of ROM " << theName;
      return false;
   }

   return true;
}

MemoryRange MemoryDev::getAddressRange()
{
   MemoryRange retVal;
   retVal.first = theAddress;
   retVal.second = theAddress + theSize - 1;
   return retVal;
}

std::string MemoryDev::getDebugString()
{
   std::string retVal = "[";
   retVal += addressToString(theAddress);
   retVal += " ~ ";
   retVal += theName;
   retVal += " ~ ";
   retVal += addressToString(theAddress + theSize - 1);
   retVal += "]";

   return retVal;
}

bool MemoryDev::isFullyConfigured()
{
   return true;
}

std::vector<std::string> MemoryDev::getIntConfigParams()
{
   std::vector<std::string> retVal;
   retVal.push_back("startAddress");
   retVal.push_back("size");
   return retVal;
}

std::vector<std::string> MemoryDev::getStringConfigParams()
{
   std::vector<std::string> retVal;
   retVal.push_back("instanceName");
   return retVal;
}

void MemoryDev::setIntConfigValue(std::string paramName, int value)
{
   LOG_DEBUG() << "Device " << theName << " config " << paramName << " = " << value;

   if (paramName == "startAddress")
   {
      theAddress = value;
   }

   if (paramName == "size")
   {
      theSize = value;
   }
}
void MemoryDev::setStringConfigValue(std::string paramName, std::string value)
{
   LOG_DEBUG() << "Device " << theName << " config " << paramName << " = " << value;

   if (paramName == "instanceName")
   {
      theName = value;
   }
}
