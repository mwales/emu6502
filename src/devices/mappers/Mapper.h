#ifndef MAPPER_H
#define MAPPER_H

#include "EmulatorCommon.h"
#include <cstdint>
#include "EmulatorConfig.h"
#include "Logger.h"

#ifdef MAPPER_TRACE
   #define MAPPER_DEBUG  LOG_DEBUG
   #define MAPPER_WARNING  LOG_WARNING
#else
   #define MAPPER_DEBUG    if(0) LOG_DEBUG
   #define MAPPER_WARNING  if(0) LOG_WARNING
#endif

/**
 * Common interface API for all the mapper classes
 */
class Mapper
{
public:
    virtual ~Mapper();

    virtual uint8_t read8(CpuAddress address) = 0;

    virtual void write8(CpuAddress address, uint8_t value) = 0;
};

#endif // MAPPER_H
