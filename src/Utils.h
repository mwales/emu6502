#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdint.h>
#include <vector>
#include "EmulatorConfig.h"

#include <SDL.h>
#include <SDL_rwops.h>

/**
 * Standalone utility functions
 *
 * @todo Need to create a toCpuAddress and parseCpuAddress #define that calls the appropriate
 *       methods / macro
 */
class Utils
{


public:

   // Convert an integer into hex string (includes 0x prefix)
   static std::string toHex8(uint8_t val, bool withPrefix = true);
   static std::string toHex16(uint16_t val, bool withPrefix = true);
   static std::string toHex32(uint32_t val, bool withPrefix = true);
   static std::string toHex64(uint64_t val, bool withPrefix = true);

   // Parse user input into integer.  If user includes "0x", parse as base 16, otherwise base 10
   static uint8_t parseUInt8(std::string userInput, bool* success = nullptr);
   static uint16_t parseUInt16(std::string userInput, bool* success = nullptr);
   static uint32_t parseUInt32(std::string userInput, bool* success = nullptr);
   static uint64_t parseUInt64(std::string userInput, bool* success = nullptr);

   /**
    * Loads an entire file into a string
    * @param name Filename to load
    * @param[out] errorOut Returns an error if one occurs.  Intialize with empty string before
    *             calling and check to see if still empty
    * @return File contents
    */
   static std::string loadFile(std::string& name, std::string& errorOut);

   /// Converts a string into a list of the string parts (whitespace removed)
   static std::vector<std::string> tokenizeSting(std::string const & input);

   /// Converts a buffer of data into "aa bb cc .."
   static std::string hexDump(uint8_t* buffer, int length);

   static int readUntilEof(uint8_t* buffer, int numBytes, SDL_RWops* fp);


   /// Is the given number a power of 2
   static bool isPowerOf2(int32_t val);

   /// Find the next power of 2 higher than the current number
   static int32_t nextPowerOf2(int32_t val);

protected:

    Utils();

};



#endif // UTILS_H
