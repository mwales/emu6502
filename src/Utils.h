#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdint.h>

class Utils
{


public:

    static std::string toHex8(uint8_t val);

    static std::string toHex16(uint16_t val);

    static std::string toHex32(uint32_t val);

protected:

    Utils();

};



#endif // UTILS_H
