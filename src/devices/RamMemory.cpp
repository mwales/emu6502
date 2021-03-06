#include <string.h>

#include "RamMemory.h"
#include "EmulatorConfig.h"
#include "Logger.h"
#include "MemoryFactory.h"
#include "force_execute.h"
#include "Utils.h"
#include "ConfigManager.h"

#ifdef RAM_TRACE
   #define RAM_DEBUG    LOG_DEBUG
   #define RAM_WARNING  LOG_WARNING
#else
   #define RAM_DEBUG    if(0) LOG_DEBUG
   #define RAM_WARNING  if(0) LOG_WARNING
#endif

// Static methods
std::string RamMemory::getTypeName()
{
   return "RAM";
}

MemoryDev* ramDeviceConstructor(std::string name)
{
   return new RamMemory(name);
}

MemoryDeviceConstructor RamMemory::getMDC()
{
   return ramDeviceConstructor;
}

RamMemory::RamMemory(std::string name):
   MemoryDev(name),
   theConfigFlags(0),
   theData(nullptr),
   theLoadDataOffset(0)
{
   RAM_DEBUG() << "Created a RAM device: " << name;

   theUint32ConfigParams.add("size", &theSize);
   theUint32ConfigParams.add("startAddress", &theAddress);
   theUint32ConfigParams.add("loadDataOffset", &theLoadDataOffset);
   theOptionalParams.add("loadDataOffset", false);
   theStrConfigParams.add("loadDataFile", &theLoadDataFile);
   theOptionalParams.add("loadDataFile", false);
}

RamMemory::~RamMemory()
{
   if (theData)
   {
      delete[] theData;
      theData = 0;
   }
}

uint8_t RamMemory::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      return 0;
   }

   return theData[absAddr - theAddress];
}

bool RamMemory::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   theData[absAddr - theAddress] = val;
   return true;
}

uint16_t RamMemory::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   uint16_t* retData = (uint16_t*) &theData[absAddr - theAddress];
   return *retData;
}

bool RamMemory::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   uint16_t* dataPtr = (uint16_t*) &theData[absAddr - theAddress];
   *dataPtr = val;
   return true;
}

// RAM configuration flags
#define RAM_ADDR_CONFIG 0x01
#define RAM_SIZE_CONFIG 0x02
#define RAM_CONFIG_DONE (RAM_ADDR_CONFIG | RAM_SIZE_CONFIG)

bool RamMemory::isFullyConfigured() const
{
   return (theConfigFlags == RAM_CONFIG_DONE);
}

std::string RamMemory::getConfigTypeName() const
{
   return getTypeName();
}

void RamMemory::resetMemory()
{
   // If the object is completely configured, initialize itself properly
   // if (isFullyConfigured())
   {
      if (theData != nullptr)
      {
         RAM_WARNING() << "Reconfiguring RAM that was already initialized once";
         delete[] theData;
      }

      theData = new uint8_t[theSize];
      memset(theData, 0, theSize);

      RAM_DEBUG() << "RAM INITIALIZED: " << theSize << " bytes "
                  << addressToString(theAddress) << "-"
                  << addressToString(theAddress + theSize - 1);
   } 
   
   if (!theLoadDataFile.empty())
   {
      // There is a file with initial RAM state to load
      std::string configStr = ConfigManager::getInstance()->getConfigDirectory();

      std::string fullPath = configStr + "/" + theLoadDataFile;

      RAM_DEBUG() << "Going to load initial RAM data from" << fullPath << " at " << addressToString(theLoadDataOffset);
      std::string errorStr;
      std::vector<uint8_t> data = Utils::loadFileBytes(fullPath, errorStr);
      
      if (!errorStr.empty())
      {
         RAM_WARNING() << "Error loading RAM data from file: " << fullPath
                       << ": " << errorStr;
         return;
      }
      
      // copy the  file contents into RAM
      CpuAddress dataWriteLoc = theLoadDataOffset;
      for(auto curByte: data)
      {
         if (dataWriteLoc >= theSize)
         {
            RAM_WARNING() << "Error loading RAM data, out of RAM";
            return;
         }
         
         theData[dataWriteLoc] = curByte;
         dataWriteLoc += 1;
      }

      RAM_DEBUG() << "Data Loaded from file:" << Utils::hexDump(data.data(), data.size());
   }
   else
   {
      RAM_DEBUG() << "No filename for data to load into RAM";
   }
}

typedef struct RamMemorySaveStruct
{
   uint32_t theMagicNumber;

   CpuAddress theAddress;
   CpuAddress theSize;

   CpuAddress theLoadDataOffset;

   // Include room for the null terminator!
   uint32_t theLoadDataFilenameLength;


} RamMemorySave;

#define RAM_MEMORY_MAGIC 0xf00dda7a

uint32_t RamMemory::getSaveStateLength()
{
   uint32_t returnVal = sizeof(RamMemorySave);
   returnVal += theLoadDataFile.size() + 1;
   returnVal += theSize;
   return returnVal;
}

bool RamMemory::saveState(uint8_t* buffer, uint32_t* bytesSaved)
{
   uint8_t* originalBuffer = buffer;

   RamMemorySave saveData;
   memset(&saveData, 0, sizeof(saveData));
   *bytesSaved = 0;

   // Fill in the contents of the save data
   saveData.theMagicNumber = RAM_MEMORY_MAGIC;
   saveData.theAddress = theAddress;
   saveData.theSize = theSize;
   // Don't care about the name
   saveData.theLoadDataOffset = theLoadDataOffset;
   saveData.theLoadDataFilenameLength = theLoadDataFile.size() + 1;

   // Start writing the buffer
   memcpy(buffer, &saveData, sizeof(RamMemorySave));
   buffer += sizeof(RamMemorySave);
   *bytesSaved += sizeof(RamMemorySave);

   // Next write the data from memory
   memcpy(buffer, theData, theSize);
   buffer += theSize;
   *bytesSaved += theSize;

   // Finally write the filename
   memcpy(buffer, theLoadDataFile.c_str(), theLoadDataFile.size());
   buffer += theLoadDataFile.size();
   * bytesSaved += theLoadDataFile.size();

   // Write the null byte
   *buffer = 0;
   buffer += 1;
   *bytesSaved += 1;

   RAM_DEBUG() << "Data saved for" << getName() << ":" << Utils::hexDump(originalBuffer, *bytesSaved);
   return true;
}

bool RamMemory::loadState(uint8_t* buffer, uint32_t* bytesLoaded)
{
   RamMemorySave saveData;
   memset(&saveData, 0, sizeof(RamMemorySave));
   memcpy(&saveData, buffer, sizeof(RamMemorySave));

   if (saveData.theMagicNumber != RAM_MEMORY_MAGIC)
   {
      std::cout << "Error loading RAM save data magic number";
      *bytesLoaded = 0;
      return false;
   }

   theAddress = saveData.theAddress;

   if (theSize != saveData.theSize)
   {
      std::cout << "RAM size currently doesn't match size of saved RAM data";
      // This way we don't have to resize the data
      return false;
   }
   theLoadDataOffset = saveData.theLoadDataOffset;

   *bytesLoaded = sizeof(RamMemorySave);
   buffer += sizeof(RamMemorySave);
   saveData.theLoadDataFilenameLength = theLoadDataFile.size() + 1;

   // Next write the data from memory
   memcpy(theData, buffer, theSize);
   buffer += theSize;
   *bytesLoaded += theSize;

   // Finally write the filename
   theLoadDataFile = std::string( (char*) buffer);
   buffer += saveData.theLoadDataFilenameLength;
   *bytesLoaded += saveData.theLoadDataFilenameLength;

   RAM_DEBUG() << "RAM data loaded for" << getName();
   return true;
}

FORCE_EXECUTE(fe_ram_memory)
{
	MemoryFactory* mf = MemoryFactory::getInstance();
	mf->registerMemoryDeviceType(RamMemory::getTypeName(), RamMemory::getMDC());
}

