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

std::string MemoryDev::getName() const
{
   return theName;
}

CpuAddress MemoryDev::getAddress() const
{
   return theAddress;
}

CpuAddress MemoryDev::getSize() const
{
   return theSize;
}

bool MemoryDev::isAbsAddressValid(CpuAddress addr, bool haltOnError) const
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

MemoryRange MemoryDev::getAddressRange() const
{
   MemoryRange retVal;
   retVal.first = theAddress;
   retVal.second = theAddress + theSize - 1;
   return retVal;
}

std::string MemoryDev::getDebugString() const
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

std::string MemoryDev::dump()
{
   CpuAddress dumpStart = (theAddress & 0xf) ^ theAddress;
   CpuAddress dumpEnd = (theAddress - 1 + theSize) | 0xf;
   CpuAddress numBytesToDump = dumpEnd - dumpStart;
   std::string retVal;

   retVal += "Dumping from address ";
   retVal += addressToString(dumpStart);
   retVal += " - ";
   retVal += addressToString(dumpEnd);
   retVal += ". Size = ";
   retVal += addressToString(numBytesToDump);
   retVal += "\n";

   CpuAddress numBytesDumped = 0;
   for(CpuAddress cur = dumpStart; numBytesDumped < numBytesToDump; cur += 1)
   {
      // Is it the start of a 16-byte line of memory
      if ( (cur & 0xf) == 0x0)
      {
         retVal += Utils::toHex16(cur);
         retVal += "  ";
      }

      // Is this address within memory
      if ( (cur >= theAddress) && ( cur <= theAddress + theSize) )
      {
         retVal += Utils::toHex8(read8(cur), false);
         retVal += " ";
      }
      else
      {
         retVal += "   ";
      }

      // Is this the end of a line?
      if ( (cur & 0xf) == 0xf)
      {
         retVal += "\n";
      }

      numBytesDumped += 1;
   }

   return retVal;
}

bool MemoryDev::specifiesStartAddress() const
{
   return false;
}

CpuAddress MemoryDev::getStartPcAddress() const
{
   return 0;
}

bool MemoryDev::isFullyConfigured() const
{
   return true;
}

std::vector<std::string> MemoryDev::getIntConfigParams() const
{
   std::vector<std::string> retVal;
   retVal.push_back("startAddress");
   retVal.push_back("size");
   return retVal;
}

std::vector<std::string> MemoryDev::getStringConfigParams() const
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
