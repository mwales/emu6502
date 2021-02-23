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

bool MemoryDev::configSelf()
{
   ConfigManager* theCfgMgr = ConfigManager::getInstance();
   bool retVal = true;

   // Get all of our int configuration parameters
   for(auto key32bit: theUint32ConfigParams.getKeys())
   {
      uint32_t temp;
      if (theCfgMgr->isConfigPresent(getConfigTypeName(), theName, key32bit))
      {
         temp = theCfgMgr->getIntegerConfigValue(getConfigTypeName(),
                                                 theName,
                                                 key32bit);
         *(theUint32ConfigParams.getValue(key32bit)) = temp;
         LOG_DEBUG() << "Config: " << getConfigTypeName() << "." << theName
                     << "." << key32bit << "=" << Utils::toHex32(temp);

         if (theOptionalParams.contains(key32bit))
         {
            theOptionalParams.set(key32bit, true);
         }
      }
      else
      {
         if (theOptionalParams.contains(key32bit))
         {
            LOG_DEBUG() << "Optional config not present:" << key32bit;
         }
         else
         {
            LOG_WARNING() << "Config Missing: " << getConfigTypeName() << "."
                          << theName << "." << key32bit;
            retVal = false;
         }
      }
   }

   // Get all of our int configuration parameters
   for(auto key16bit: theUint16ConfigParams.getKeys())
   {
      uint16_t temp;
      if (theCfgMgr->isConfigPresent(getConfigTypeName(), theName, key16bit))
      {
         temp = theCfgMgr->getIntegerConfigValue(getConfigTypeName(),
                                                 theName,
                                                 key16bit);
         *(theUint16ConfigParams.getValue(key16bit)) = temp;
         LOG_DEBUG() << "Config: " << getConfigTypeName() << "." << theName
                     << "." << key16bit << "=" << Utils::toHex16(temp);

         if (theOptionalParams.contains(key16bit))
         {
            theOptionalParams.set(key16bit, true);
         }
      }
      else
      {
         if (theOptionalParams.contains(key16bit))
         {
            LOG_DEBUG() << "Optional config not present:" << key16bit;
         }
         else
         {
            LOG_WARNING() << "Config Missing: " << getConfigTypeName() << "."
                          << theName << "." << key16bit;
            retVal = false;
         }
      }
   }
   
   // Get all of our string configuration parameters
   for(auto keyStr: theStrConfigParams.getKeys())
   {
      std::string temp;
      if (theCfgMgr->isConfigPresent(getConfigTypeName(), theName, keyStr))
      {
         temp = theCfgMgr->getStringConfigValue(getConfigTypeName(),
                                                theName,
                                                keyStr);
         *(theStrConfigParams.getValue(keyStr)) = temp;
         LOG_DEBUG() << "Config: " << getConfigTypeName() << "." << theName
                     << "." << keyStr << "=" << temp;

         if (theOptionalParams.contains(keyStr))
         {
            theOptionalParams.set(keyStr, true);
         }
      }
      else
      {
         if (theOptionalParams.contains(keyStr))
         {
            LOG_DEBUG() << "Optional config not present:" << keyStr;
         }
         else
         {
            LOG_WARNING() << "Config Missing: " << getConfigTypeName() << "."
                          << theName << "." << keyStr;
            retVal = false;
         }
      }
   }

   return retVal;
}

 std::string MemoryDev::getMemoryConfigHelp()
{
   std::string retVal;
   retVal += "Memory Device Type: ";
   retVal += getConfigTypeName();
      
   bool isFirst = true;
   for(auto curConfig: theUint16ConfigParams.getKeys())
   {
      if (isFirst)
      {
         retVal += "\n    16-bit integer config: ";
         isFirst = false;
      }
      else
      {
         retVal += ", ";
      }
      retVal += curConfig;

      if (theOptionalParams.contains(curConfig))
      {
          retVal += " (optional)";
      }
   }
      
   isFirst = true;
   for(auto curConfig: theUint32ConfigParams.getKeys())
   {
      if (isFirst)
      {
         retVal += "\n    32-bit integer config: ";
         isFirst = false;
      }
      else
      {
         retVal += ", ";
      }
      retVal += curConfig;

      if (theOptionalParams.contains(curConfig))
      {
          retVal += " (optional)";
      }
   }
      
   isFirst = true;
   for(auto curConfig: theStrConfigParams.getKeys())
   {
      if (isFirst)
      {
         retVal += "\n    String config: ";
         isFirst = false;
      }
      else
      {
         retVal += ", ";
      }
      retVal += curConfig;

      if (theOptionalParams.contains(curConfig))
      {
          retVal += " (optional)";
      }
   }
   retVal += "\n";
   
   return retVal;
}

 uint32_t MemoryDev::getSaveStateLength()
 {
    return 0;
 }

 bool MemoryDev::saveState(uint8_t* buffer, uint32_t* bytesSaved)
 {
    *bytesSaved = 0;
    return true;
 }

 bool MemoryDev::loadState(uint8_t* buffer, uint32_t* bytesLoaded)
 {
    *bytesLoaded = 0;
    return true;
 }


