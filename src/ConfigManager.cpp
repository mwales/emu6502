#include "ConfigManager.h"
#include "Logger.h"
#include "EmulatorConfig.h"
#include "Utils.h"


#ifdef CONFIG_MANAGER_DEBUG
   #define CFGMGR_DEBUG  LOG_DEBUG
   #define CFGMGR_WARNING  LOG_WARNING
#else
   #define CFGMGR_DEBUG    if(0) LOG_DEBUG
   #define CFGMGR_WARNING  if(0) LOG_WARNING
#endif

ConfigManager* ConfigManager::theInstance = nullptr;

ConfigManager::ConfigManager()
{
   CFGMGR_DEBUG() << "Creating a ConfigManager instance";
}

ConfigManager* ConfigManager::getInstance()
{
   if (!theInstance)
   {
      LOG_FATAL() << "Can't ask for instance of ConfigManager before it has been created";
      return nullptr;
   }

   return theInstance;
}

ConfigManager* ConfigManager::createInstance()
{
   if (theInstance != nullptr)
   {
      LOG_FATAL() << "Will not create instance where there is already ConfigManager instance";
      return nullptr;
   }

   theInstance = new ConfigManager();
   return theInstance;
}

void ConfigManager::destroyInstance()
{
   if(!theInstance)
   {
      CFGMGR_WARNING() << "ConfigManager instance already null when destroyInstance called";
   }
   else
   {
      CFGMGR_DEBUG() << "Destorying the config manager";
      delete theInstance;
      theInstance = nullptr;
   }
}

void ConfigManager::processArgs(int argc, char** argv)
{

   for(int i = 1; i < argc; i++)
   {
      processSingleArg(argv[i]);
   }
}

bool ConfigManager::isConfigTypePresent(std::string const & typeName)
{
   for(auto const & curConfig : theConfigData)
   {
      if (curConfig.theType == typeName)
      {
         CFGMGR_DEBUG() << "isConfigTypePresent(" << typeName << ") = true";
         return true;
      }
   }

   CFGMGR_DEBUG() << "isConfigTypePresent(" << typeName << ") = false";
   return false;
}

std::set<std::string> ConfigManager::getConfigTypeInstanceNames(std::string const & typeName)
{
   CFGMGR_DEBUG() << "getConfigTypeInstanceNames(" << typeName << ") called";

   std::set<std::string> retVal;
   for(auto const & curConfig: theConfigData)
   {
      // CFGMGR_DEBUG() << "Checking " << curConfig.theType << "." << curConfig.theName;

      if (curConfig.theType == typeName)
      {
         // Does this already exist in retVal
         if (retVal.count(curConfig.theName) == 0)
         {
            CFGMGR_DEBUG() << "getConfigTypeInstanceName(" << typeName << ") found" << curConfig.theName;
            retVal.insert(curConfig.theName);
         }
         else
         {
            CFGMGR_DEBUG() << "Found another" << typeName << ", but already in the set";
         }
      }
   }

   CFGMGR_DEBUG() << "getConfigTypeInstanceName(" << typeName << ") returning " << retVal.size()
                  << " instances";
   return retVal;
}

std::set<std::string> ConfigManager::getConfigTypeNames()
{
   std::set<std::string> retVal;
   for(auto const & cde : theConfigData)
   {
      retVal.insert(cde.theType);
   }

   return retVal;
}

bool ConfigManager::isConfigPresent(std::string const & typeName,
                     std::string const & instanceName,
                     std::string const & memberName)
{
   for(auto const & curConfig: theConfigData)
   {
      if ( (curConfig.theType == typeName) &&
           (curConfig.theName == instanceName) &&
           (curConfig.theMember == memberName) )
      {
         CFGMGR_DEBUG() << "isConfigPresent(" << typeName << "," << instanceName << ","
                        << memberName << ") = true";
         return true;
      }
   }

   CFGMGR_DEBUG() << "isConfigPresent(" << typeName << "," << instanceName << ","
                  << memberName << ") = false";
   return false;
}

uint32_t ConfigManager::getIntegerConfigValue(std::string const & typeName,
                               std::string const & instanceName,
                               std::string const & memberName)
{
   CFGMGR_DEBUG() << "getIntegerConfigValue(" << typeName << "," << instanceName << ","
                  << memberName << ") called";

   for(auto const & curConfig: theConfigData)
   {
      if ( (curConfig.theType == typeName) &&
           (curConfig.theName == instanceName) &&
           (curConfig.theMember == memberName) )
      {
         bool success = false;
         uint32_t retVal = Utils::parseUInt32(curConfig.theValue, &success);

         if (success)
         {
            CFGMGR_DEBUG() << "Returning " << curConfig.theValue << " = " << retVal;
            return retVal;
         }
         else
         {
            LOG_WARNING() << "Return 0, failed to convert " << curConfig.theValue << " to integer";
            return 0;
         }
      }
   }

   // We never found the member indicated
   CFGMGR_WARNING() << "Return 0, configuration value not found";
   return 0;
}

std::string ConfigManager::getStringConfigValue(std::string const & typeName,
                                 std::string const & instanceName,
                                 std::string const & memberName)
{
   CFGMGR_DEBUG() << "getStringConfigValue(" << typeName << "," << instanceName << ","
                  << memberName << ") called";

   for(auto const & curConfig: theConfigData)
   {
      if ( (curConfig.theType == typeName) &&
           (curConfig.theName == instanceName) &&
           (curConfig.theMember == memberName) )
      {
         CFGMGR_DEBUG() << "Returning " << curConfig.theValue;
         return curConfig.theValue;
      }
   }

   // We never found the member indicated
   CFGMGR_WARNING() << "Return empty string, configuration value not found";
   return 0;
}

void ConfigManager::loadConfigFile(std::string filename)
{
   std::string errorString;
   std::string fileContents = Utils::loadFile(filename, errorString);

   if(errorString != "")
   {
      // Failed to read th efile
      CFGMGR_WARNING() << "Error loading the config file:" << errorString;
      return;
   }

   std::vector<std::string> configTokens = Utils::tokenizeString(fileContents);

   CFGMGR_DEBUG() << "Found" << configTokens.size() << "tokens in config file";

   for(auto const & singleToken: configTokens)
   {
      processSingleArg(singleToken);
   }

   CFGMGR_DEBUG() << "Done processing config file:" << filename;
}

bool ConfigManager::isConfigPresent(std::string const & typeName, std::string const & memberName)
{
   for(auto const & curConfig: theConfigData)
   {
      if ( (curConfig.theType == typeName) &&
           (curConfig.theMember == memberName) )
      {
         CFGMGR_DEBUG() << "isConfigPresent(" << typeName << "," << memberName << ") = true;";
         return true;
      }
   }

   CFGMGR_DEBUG() << "isConfigPresent(" << typeName << "," << memberName << ") = false;";
   return false;
}

uint32_t ConfigManager::getIntegerConfigValue(std::string const & typeName,
                                              std::string const & memberName)
{
   for(auto const & curConfig: theConfigData)
   {
      if ( (curConfig.theType == typeName) &&
           (curConfig.theMember == memberName) )
      {
         bool success = false;
         uint32_t retVal = Utils::parseUInt32(curConfig.theValue, &success);

         if(success)
         {
            CFGMGR_DEBUG() << "getIntegerConfigValue(" << typeName << "," << memberName << ") ="
                           << Utils::toHex32(retVal) << "=" << retVal;
            return retVal;
         }
         else
         {
            CFGMGR_WARNING() << "getIntegerConfigValue(" << typeName << "," << memberName << ") ="
                           << curConfig.theValue << " (not integer)";
         }
      }
   }

   CFGMGR_DEBUG() << "getIntegerConfigValue(" << typeName << "," << memberName
                  << ") failed.  Config not found.";
   return false;
}

std::string ConfigManager::getStringConfigValue(std::string const & typeName,
                                                std::string const & memberName)
{
   for(auto const & curConfig: theConfigData)
   {
      if ( (curConfig.theType == typeName) &&
           (curConfig.theMember == memberName) )
      {
         CFGMGR_DEBUG() << "getStringConfigValue(" << typeName << "," << memberName << "="
                        << curConfig.theValue;
         return curConfig.theValue;
      }
   }

   CFGMGR_DEBUG() << "getStringConfigValue(" << typeName << "," << memberName << ") failed";
   return "";
}

void ConfigManager::processSingleArg(std::string const & arg)
{
   CFGMGR_DEBUG() << "Processing Config Arg: " << arg;
   // Arg format type.name.member=value.  They all need two periods, one equals

   auto firstPeriodPos = arg.find(".");
   if (firstPeriodPos == std::string::npos)
   {
      LOG_DEBUG() << "No period found in config: " << arg;
      return;
   }

   auto secondPeriodPos = arg.find(".", firstPeriodPos + 1);
   if (secondPeriodPos == std::string::npos)
   {
      LOG_DEBUG() << "Couldn't find second period in configuraiton: " << arg;
      return;
   }

   auto equalsPos = arg.find("=");
   if (equalsPos == std::string::npos)
   {
      LOG_DEBUG() << "Couldn't find the equal sign in configuration: " << arg;
      return;
   }

   ConfigDataEntry cde;
   cde.theType = arg.substr(0, firstPeriodPos);
   cde.theName = arg.substr(firstPeriodPos + 1, secondPeriodPos - firstPeriodPos - 1);
   cde.theMember = arg.substr(secondPeriodPos + 1, equalsPos - secondPeriodPos - 1);
   cde.theValue = arg.substr(equalsPos + 1);

   CFGMGR_DEBUG() << "  Type=" << cde.theType << ", Name=" << cde.theName
                  << ", Member=" << cde.theMember << ", Value=" << cde.theValue;

   theConfigData.push_back(cde);
}
