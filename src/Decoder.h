#ifndef DECODER_H
#define DECODER_H

#include<stdint.h>

class MemoryController;

class Decoder
{

protected:

   Decoder(MemoryController* memCtrl)
   {
      theMemoryController = memCtrl;
   }

public:
   virtual void start(uint16_t address) = 0;


protected:

   MemoryController* theMemoryController;

};

#endif // DECODER_H
