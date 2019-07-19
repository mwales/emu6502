#include "MemoryConfig.h"
#include "MemoryDev.h"


#include "Logger.h"
#include "Utils.h"

#define CONFIG_TRACE

#ifdef CONFIG_TRACE
   #define CFG_DEBUG    LOG_DEBUG
   #define CFG_WARNING  LOG_WARNING
#else
   #define CFG_DEBUG    if(0) LOG_DEBUG
   #define CFG_WARNING  if(0) LOG_WARNING
#endif

MemoryConfig::MemoryConfig(MemoryController* mc):
   theMemoryController(mc),
   theStartAddressSet(false),
   theStartAddress(0xfffe)
{
   // Empty
}

bool MemoryConfig::parseConfig(std::string configJson)
{
   CFG_DEBUG() << "Parsing " << configJson.length() << " bytes of configuration data";

   // Parse the JSON configuration file
   cJSON* json = cJSON_Parse(configJson.c_str());
   if (json == nullptr)
   {
      char const * errorStr = cJSON_GetErrorPtr();
      if (errorStr == nullptr)
      {
         LOG_FATAL() << "JSON error parsing configuration file, unknown error, FUBAR";
         return false;
      }
      else
      {
         LOG_WARNING() << "JSON error: " << errorStr;
         return false;
      }
   }

   // Get the configuration name
   cJSON* j = cJSON_GetObjectItem(json, "configName");
   if ( (j == nullptr) || !cJSON_IsString(j) )
   {
      CFG_DEBUG() << "No valid configuration name given";
      theConfigurationName = "NO_CONFIG_NAME_GIVEN";
   }
   else
   {
      theConfigurationName = j->valuestring;
      CFG_DEBUG() << "Configuration Name is" << theConfigurationName;
   }

   // Get the start address for execution
   j = cJSON_GetObjectItem(json, "startAddress");
   if ( (j == nullptr) || !cJSON_IsNumber(j) )
   {
      CFG_WARNING() << "No start address specified in the configuration file";
   }
   else
   {
      theStartAddress = j->valueint;
      theStartAddressSet = true;
      CFG_DEBUG() << "JSON configuration specified start address of" << addressToString(theStartAddress);
   }

   // Get the start address for execution
   j = cJSON_GetObjectItem(json, "displayType");
   if ( (j == nullptr) || !cJSON_IsString(j) )
   {
      CFG_DEBUG() << "No display type specified in the configuration file";
      theDisplayType = "none";
   }
   else
   {
      theDisplayType = j->valuestring;
      CFG_DEBUG() << "JSON configuration specified display type " << theDisplayType;
   }

   // Parse the array of devices
   j = cJSON_GetObjectItem(json, "devices");
   if ( (j == nullptr) || (!cJSON_IsArray(j)))
   {
      CFG_WARNING() << "JSON configuration file doesn't have a valid deviceName configuration array";
   }
   else
   {
      parseDeviceArray(j);
   }

   CFG_DEBUG() << "Deleting JSON object";
   cJSON_Delete(json);

   return true;
}

void MemoryConfig::parseDeviceArray(cJSON* j)
{
   int arraySize = cJSON_GetArraySize(j);
   CFG_DEBUG() << "parseDeviceArray called on array size " << arraySize;

   for(int i = 0; i < arraySize; i++)
   {
      cJSON* ai = cJSON_GetArrayItem(j, i);

      if ( (ai == nullptr) || !cJSON_IsObject(ai) )
      {
         char const * errorStr = cJSON_GetErrorPtr();
         if (errorStr == nullptr)
         {
            CFG_WARNING() << "Failed to parse a device in the configuration JSON file, unknown error";
         }
         else
         {
            LOG_WARNING() << "Failed to parse a device in the configuration JSON file, error: " << errorStr;
         }
      }
      else
      {
         // Parse the device details
         parseSingleDevice(ai);
      }
   }
}

void MemoryConfig::parseSingleDevice(cJSON* j)
{
   // Each device should have a type, and instanceName

   CFG_DEBUG() << "parseSingleDevice called";

   cJSON* jType = cJSON_GetObjectItem(j, "type");
   cJSON* jInstanceName = cJSON_GetObjectItem(j, "instanceName");

   if ( (jType == nullptr) || !cJSON_IsString(jType) )
   {
        CFG_WARNING() << "Failed to parse a valid type for the device";
        return;
   }

   if ( (jInstanceName == nullptr) || !cJSON_IsString(jInstanceName) )
   {
        CFG_WARNING() << "Failed to parse a valid instance name for the device";
        return;
   }

   std::string typeName = jType->valuestring;
   std::string instanceName = jInstanceName->valuestring;

   auto fIter = theDevConstructorList.find(typeName);
   if (fIter == theDevConstructorList.end() )
   {
      CFG_WARNING() << "Device Type " << typeName << " (" << instanceName << ") not found in list";
      return;
   }

   CFG_DEBUG() << "Constructing a " << typeName << " device named " << instanceName;
   MemoryDeviceConstructor devConstructor = *fIter->second;
   MemoryDev* md = (devConstructor)(instanceName);

   std::vector<std::string> intParamList = md->getIntConfigParams();
   for(auto const paramName: intParamList)
   {
      cJSON* jInt = cJSON_GetObjectItem(j, paramName.c_str());
      if ( (jInt != nullptr) && cJSON_IsNumber(jInt) )
      {
         md->setIntConfigValue(paramName, jInt->valueint);
      }

   }

   std::vector<std::string> strParamList = md->getStringConfigParams();
   for(auto const paramName: strParamList)
   {
      cJSON* jStr = cJSON_GetObjectItem(j, paramName.c_str());
      if ( (jStr != nullptr) && cJSON_IsString(jStr) )
      {
         md->setStringConfigValue(paramName, jStr->valuestring);
      }
   }

   if (md->isFullyConfigured())
   {
      CFG_DEBUG() << "Device " << instanceName << " fully configured, adding to memory controller";
      md->setMemoryController(theMemoryController);
      md->resetMemory();
      theMemoryController->addNewDevice(md);
   }
   else
   {
      CFG_WARNING() << "Device " << instanceName << " not fully configured!  Not adding to system";
   }

}

void MemoryConfig::registerMemoryDevice(std::string deviceName, MemoryDeviceConstructor mdc)
{
   theDevConstructorList[deviceName] = mdc;
}

std::string MemoryConfig::getConfigurationName()
{
   return theConfigurationName;
}

CpuAddress MemoryConfig::getStartAddress()
{
   return theStartAddress;
}

std::string MemoryConfig::getDisplayType()
{
   return theDisplayType;
}

bool MemoryConfig::isStartAddressSet()
{
   return theStartAddressSet;
}
