#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdint.h>

class Utils
{


public:

   // Convert an integer into hex string (includes 0x prefix)
   static std::string toHex8(uint8_t val);
   static std::string toHex16(uint16_t val);
   static std::string toHex32(uint32_t val);

   // Parse user input into integer.  If user includes "0x", parse as base 16, otherwise base 10
   static uint8_t parseUInt8(std::string userInput, bool* success = nullptr);
   static uint16_t parseUInt16(std::string userInput, bool* success = nullptr);
   static uint32_t parseUInt32(std::string userInput, bool* success = nullptr);


protected:

    Utils();

};



#endif // UTILS_H
