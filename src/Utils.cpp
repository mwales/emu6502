#include "Utils.h"

Utils::Utils()
{

}

std::string Utils::toHex8(uint8_t val, bool withPrefix)
{
   char buf[5];

   if (withPrefix)
      snprintf(buf, 5, "0x%02x", val);
   else
      snprintf(buf, 5, "%02x", val);

   buf[4] = 0;

   return buf;
}


std::string Utils::toHex16(uint16_t val, bool withPrefix)
{
   char buf[7];

   if(withPrefix)
      snprintf(buf, 7, "0x%04x", val);
   else
      snprintf(buf, 7, "%04x", val);

   buf[6] = 0;

   return buf;
}

std::string Utils::toHex32(uint32_t val, bool withPrefix)
{
   char buf[11];

   if(withPrefix)
      snprintf(buf, 11, "0x%08x", val);
   else
      snprintf(buf, 11, "%08x", val);
   buf[10] = 0;

   return buf;
}


uint8_t Utils::parseUInt8(std::string userInput, bool* success)
{
   uint32_t val = parseUInt32(userInput, success);
   if ( (val > UINT8_MAX) && (success != nullptr) )
   {
     *success = false;
   }
   return (uint8_t) val;
}

uint16_t Utils::parseUInt16(std::string userInput, bool* success)
{
   uint32_t val = parseUInt32(userInput, success);
   if ( (val > UINT16_MAX) && (success != nullptr) )
   {
     *success = false;
   }
   return (uint16_t) val;
}

uint32_t Utils::parseUInt32(std::string userInput, bool* success)
{
   errno = 0;
   uint32_t val = 0;
   char const * userInputCharStar = userInput.c_str();

   if ( (userInput.size() > 2) &&
        (userInput[0] == '0') &&
        (userInput[1] == 'x') )
   {
      // User provided base 16 number
      val = strtoul(userInputCharStar + 2, 0, 16);
   }
   else
   {
      // User provided base 16 number
      val = strtoul(userInputCharStar, 0, 10);
   }

   if ( (errno != 0) && (success != nullptr) )
   {
      *success = false;
   }
   else if (success != nullptr)
   {
      *success = true;
   }

   return val;
}
