#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "Decoder.h"
#include <iostream>
#include <map>


class Disassembler : public Decoder
{
public:
   Disassembler(MemoryController* memCtrl);

   virtual void start(CpuAddress address);

   virtual void halt();

   void printDisassembly();

protected:

   std::map<CpuAddress, std::string> theListing;

   std::map<CpuAddress, std::string> theLabels;

   bool theHaltFlag;

};


#endif // DISASSEMBLER_H
