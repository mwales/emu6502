#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "Decoder.h"


class Disassembler : public Decoder
{
public:
   Disassembler(MemoryController* memCtrl);

   virtual void start(uint16_t address) = 0;

};


#endif // DISASSEMBLER_H
