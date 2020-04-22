#include"MemoryDev.h"
#include "Logger.h"
#include "Utils.h"
#include "ConfigManager.h"

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

std::string MemoryDev::dump(bool asciiDump)
{
   CpuAddress dumpStart = theAddress - (theAddress & 0xf);
   CpuAddress dumpEnd = (theAddress + theSize - 1) | 0xf ;
   CpuAddress numBytesToDump = dumpEnd - dumpStart + 1;
   std::string retVal;
   std::string asciiText;

   retVal += "Dumping ";
   retVal += theName;
   retVal += " from address ";
   retVal += addressToString(theAddress);
   retVal += " - ";
   retVal += addressToString(theAddress + theSize - 1);
   retVal += ". Size = ";
   retVal += addressToString(theSize);
   retVal += "\n";

   CpuAddress numBytesDumped = 0;
   for(CpuAddress cur = dumpStart; numBytesDumped < numBytesToDump; cur += 1)
   {
      // Is it the start of a 16-byte line of memory
      if ( (cur & 0xf) == 0x0)
      {
         retVal += Utils::toHex16(cur);
         retVal += "  ";
         asciiText = "|";
      }

      // Is this address within memory
      if ( (cur >= theAddress) && ( cur < theAddress + theSize) )
      {
         uint8_t memVal = read8(cur);
         retVal += Utils::toHex8(memVal, false);
         retVal += " ";

         if ( (memVal >= 0x20) && (memVal <= 0x7e) )
         {
            // It's ascii printable
            asciiText += (char) memVal;
         }
         else
         {
            // Write a . for non-printable characters
            asciiText += ".";
         }
      }
      else
      {
         retVal += "   ";
         asciiText += " ";
      }

      // Put a space between bytes 7 and 8
      if ( (cur & 0xf) == 0x7)
      {
         retVal += " ";
      }

      // Is this the end of a line?
      if ( (cur & 0xf) == 0xf)
      {
         if (asciiDump)
         {
            retVal += "   ";
            retVal += asciiText;
            retVal += "|";
         }
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

bool MemoryDev::configSelf()
{
   ConfigManager* theCfgMgr = ConfigManager::getInstance();
   bool retVal = true;

   // Get all of our int configuration parameters
   for(auto it = theUint32ConfigParams.begin(); it != theUint32ConfigParams.end(); it++)
   {
      uint32_t temp;
      if (theCfgMgr->isConfigPresent(getConfigTypeName(), theName, it->first))
      {
         temp = theCfgMgr->getIntegerConfigValue(getConfigTypeName(),
                                                 theName,
                                                 it->first);
         *(it->second) = temp;
         LOG_DEBUG() << "Config: " << getConfigTypeName() << "." << theName
                     << "." << it->first << "=" << Utils::toHex32(temp);
      }
      else
      {
         LOG_WARNING() << "Config Missing: " << getConfigTypeName() << "."
                       << theName << "." << it->first;
         retVal = false;
      }
   }

   // Get all of our int configuration parameters
   for(auto it = theUint16ConfigParams.begin(); it != theUint16ConfigParams.end(); it++)
   {
      uint16_t temp;
      if (theCfgMgr->isConfigPresent(getConfigTypeName(), theName, it->first))
      {
         temp = theCfgMgr->getIntegerConfigValue(getConfigTypeName(),
                                                 theName,
                                                 it->first);
         *(it->second) = temp;
         LOG_DEBUG() << "Config: " << getConfigTypeName() << "." << theName
                     << "." << it->first << "=" << Utils::toHex16(temp);
      }
      else
      {
         LOG_WARNING() << "Config Missing: " << getConfigTypeName() << "."
                       << theName << "." << it->first;
         retVal = false;
      }
   }
   // Get all of our string configuration parameters
   for(auto it = theStrConfigParams.begin(); it != theStrConfigParams.end(); it++)
   {
      std::string temp;
      if (theCfgMgr->isConfigPresent(getConfigTypeName(), theName, it->first))
      {
         temp = theCfgMgr->getStringConfigValue(getConfigTypeName(),
                                                theName,
                                                it->first);
         *(it->second) = temp;
         LOG_DEBUG() << "Config: " << getConfigTypeName() << "." << theName
                     << "." << it->first << "=" << temp;
      }
      else
      {
         LOG_WARNING() << "Config Missing: " << getConfigTypeName() << "."
                       << theName << "." << it->first;
         retVal = false;
      }
   }

   return retVal;
}


