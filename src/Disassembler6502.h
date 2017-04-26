#ifndef DISASSEMBLER6502_H
#define DISASSEMBLER6502_H

#include "Decoder6502.h"
#include <iostream>
#include <map>


class Disassembler6502 : public Decoder6502
{
public:
   Disassembler6502(MemoryController* memCtrl);

   virtual void start(CpuAddress address);

   virtual void halt();

   void printDisassembly();

   void includeOpCodes(bool val);

   void includeAddress(bool val);

protected:

   // All the virtual void methods inherited from parent decoder
   virtual void load(CpuAddress instAddr, uint8_t opCodes);
   virtual void store(CpuAddress instAddr, uint8_t opCodes);
   virtual void transfer(CpuAddress instAddr, uint8_t opCodes);
   virtual void add(CpuAddress instAddr, uint8_t opCodes);
   virtual void subtract(CpuAddress instAddr, uint8_t opCodes);
   virtual void increment(CpuAddress instAddr, uint8_t opCodes);
   virtual void decrement(CpuAddress instAddr, uint8_t opCodes);
   virtual void andOperation(CpuAddress instAddr, uint8_t opCodes);
   virtual void orOperation(CpuAddress instAddr, uint8_t opCodes);
   virtual void xorOperation(CpuAddress instAddr, uint8_t opCodes);
   virtual void shiftLeft(CpuAddress instAddr, uint8_t opCodes);
   virtual void shiftRight(CpuAddress instAddr, uint8_t opCodes);
   virtual void rotateLeft(CpuAddress instAddr, uint8_t opCodes);
   virtual void rotateRight(CpuAddress instAddr, uint8_t opCodes);
   virtual void push(CpuAddress instAddr, uint8_t opCodes);
   virtual void pull(CpuAddress instAddr, uint8_t opCodes);
   virtual void jump(CpuAddress instAddr, uint8_t opCodes);
   virtual void jumpSubroutine(CpuAddress instAddr, uint8_t opCodes);
   virtual void returnFromInterrupt(CpuAddress instAddr, uint8_t opCodes);
   virtual void returnFromSubroutine(CpuAddress instAddr, uint8_t opCodes);
   virtual void bitTest(CpuAddress instAddr, uint8_t opCodes);
   virtual void compare(CpuAddress instAddr, uint8_t opCodes);
   virtual void clear(CpuAddress instAddr, uint8_t opCodes);
   virtual void set(CpuAddress instAddr, uint8_t opCodes);
   virtual void noOp(CpuAddress instAddr, uint8_t opCodes);
   virtual void breakOperation(CpuAddress instAddr, uint8_t opCodes);
   virtual void branch(CpuAddress instAddr, uint8_t opCodes);

   std::string getOperandText(CpuAddress addr, uint8_t opcode);
   std::string getImmediateOpText(CpuAddress addr);
   std::string getAbsoluteOpText(CpuAddress addr);

   virtual void updatePc(uint8_t bytesIncrement);

   void printOpCodes(std::string* listingText, CpuAddress addr, int numOpCodes);

   void printAddress(std::string* listingText, CpuAddress addr);

   bool theDeadEndFlag;
   CpuAddress thePc;

   std::vector<CpuAddress> theEntryPoints;

   std::map<CpuAddress, std::string> theListing;

   std::map<CpuAddress, std::string> theLabels;

   bool theHaltFlag;

   bool thePrintOpCodeFlag;

   bool thePrintAddressFlag;

};


#endif // DISASSEMBLER6502_H
