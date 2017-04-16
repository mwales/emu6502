#ifndef DECODER_H
#define DECODER_H

#include<stdint.h>
#include"Cpu6502Defines.h"

class MemoryController;

class Decoder
{

public:

   /**
    * Start the decoding process
    *
    * @param address
    */
   virtual void start(CpuAddress address) = 0;

   /**
    * Something happend during decoding this callback provides a way to stop the decoding
    */
   virtual void halt() = 0;


protected:

   MemoryController* theMemoryController;

};

#endif // DECODER_H
