#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdint.h>
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

   static std::string loadFile(std::string& name, std::string& errorOut);

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
