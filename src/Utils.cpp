#include "Utils.h"

Utils::Utils()
{

}

std::string Utils::toHex8(uint8_t val)
{
   char buf[5];
   snprintf(buf, 5, "0x%02x", val);
   buf[4] = 0;

   return buf;
}


std::string Utils::toHex16(uint16_t val)
{
   char buf[7];
   snprintf(buf, 7, "0x%04x", val);
   buf[6] = 0;

   return buf;
}

std::string Utils::toHex32(uint32_t val)
{
   char buf[11];
   snprintf(buf, 11, "0x%08x", val);
   buf[10] = 0;

   return buf;
}
