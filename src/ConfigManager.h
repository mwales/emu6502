#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <iostream>
#include <vector>
#include <stdint.h>
#include <set>

/**
 * Loads the configuration from a file or from command line arguments.  Generally just ignores
 * stuff it doesn't understand.
 * 
 * Configuration params can be on the command line or in a configuration file.  The
 * configuration data can be retrieved from this singleton by asking for a string or integer
 * configuration value.
 * 
 * Format is:
 * typeName.instanceName.memberName=value
 * 
 * The config filename needs to be specified as a configuration parameter itself, it should be
 * config.anythingUnique.filename=pathToFile
 */
class ConfigManager
{
public:

   // Methods for managing the lifetime of the singleton object
   static ConfigManager* getInstance();
   static ConfigManager* createInstance();
   static void destroyInstance();

   /// Process command line arguments
   void processArgs(int argc, char** argv);

   /// Adds to the configuration by reading the contents of a configuration file
   void loadConfigFile(std::string filename);

   // Methods to check for the configuration data

   bool isConfigTypePresent(std::string const & typeName);

   std::set<std::string> getConfigTypeInstanceNames(std::string const & typeName);

   std::set<std::string> getConfigTypeNames();

   bool isConfigPresent(std::string const & typeName,
                        std::string const & instanceName,
                        std::string const & memberName);


   uint32_t getIntegerConfigValue(std::string const & typeName,
                                  std::string const & instanceName,
                                  std::string const & memberName);

   std::string getStringConfigValue(std::string const & typeName,
                                    std::string const & instanceName,
                                    std::string const & memberName);

   // The following config methods are for config that doesn't care about instance name

   bool isConfigPresent(std::string const & typeName,
                        std::string const & memberName);

   uint32_t getIntegerConfigValue(std::string const & typeName,
                                  std::string const & memberName);

   std::string getStringConfigValue(std::string const & typeName,
                                    std::string const & memberName);
   
   std::string getConfigDirectory();

protected:

   typedef struct ConfigDataEntryStruct
   {
      std::string theType;
      std::string theName;
      std::string theMember;
      std::string theValue;
   } ConfigDataEntry;

   ConfigManager();

   void processSingleArg(std::string const & arg);

   static ConfigManager* theInstance;

   std::vector<ConfigDataEntry> theConfigData;
   
   std::string theConfigPath;
};

#endif // CONFIGMANAGER_H
