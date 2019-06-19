#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdint.h>
#include "EmulatorConfig.h"

#ifdef SDL_INCLUDED
   #include <SDL2/SDL.h>
   #include <SDL2/SDL_rwops.h>
#endif

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

   static std::string loadFile(std::string& name, std::string& errorOut);

#ifdef SDL_INCLUDED
   static int readUntilEof(uint8_t* buffer, int numBytes, SDL_RWops* fp);

#endif


protected:

    Utils();

};



#endif // UTILS_H
