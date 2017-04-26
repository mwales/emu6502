#include<iostream>

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include "RomMemory.h"
#include "MemoryController.h"
#include "Disassembler6502.h"
#include "Logger.h"

int main(int argc, char* argv[])
{
   //sleep(3);

   if (argc != 3)
   {
      std::cerr << "Usage error" << std::endl;
      std::cerr << "Usage: " << argv[0] << " file.bin address" << std::endl;
      return 1;
   }

   uint32_t address32;

   if ( (argv[2][0] == '0') && (argv[2][1] == 'x') )
   {
      // User specified the address in hex
      address32 = strtoul(argv[2] + 2, 0, 16);
   }
   else
   {
      // Assume user specified decimal
      address32 = strtoul(argv[2], 0, 10);
   }

   if (address32 > UINT16_MAX)
   {
      LOG_WARNING() << "Address " << Utils::toHex32(address32) << " out of range!";
      return 1;
   }

   uint16_t addr = address32;
   LOG_DEBUG() << "Disassemling " << argv[1] << " at address " << addressToString(addr);

   RomMemory programData(argv[1], addr);

   MemoryController memControl;
   memControl.addNewDevice( (MemoryDev*) &programData);

   Disassembler6502 dis(&memControl);
   dis.includeOpCodes(true);
   dis.includeAddress(true);

   dis.start(addr);


   dis.printDisassembly();


   return 0;
}

