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
    */
   virtual void start() = 0;

   /**
    * Something happend during decoding this callback provides a way to stop the decoding
    */
   virtual void halt() = 0;

   /**
    * Setting the address for the decoder to fetch instructions at
    * @param address Entry point
    */
   virtual void setAddress(CpuAddress address)
   {
      thePc = address;
   }

   /**
    * Decode a single instruction
    * @return -1 on error, clock cycles for operation if emulating
    */
   virtual int decode() = 0;

protected:

   /**
    * The manager for accessing all memory devices
    */
   MemoryController* theMemoryController;

   /**
    * Current address of decoding (whether emulating or disassembling)
    */
   CpuAddress thePc;

};

#endif // DECODER_H
