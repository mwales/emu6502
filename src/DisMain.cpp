#include<iostream>

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<getopt.h>
#include "RomMemory.h"
#include "MemoryController.h"
#include "Disassembler6502.h"
#include "Logger.h"

void printUsage(char* appName)
{
   std::cout << appName << " is 6502 disassembler" << std::endl;
   std::cout << appName << " -f filename -b baseaddr [-a address] [-h] [-o] [-l]" << std::endl;
   std::cout << "  -f --file       Filename of the file to disassemble" << std::endl;
   std::cout << "  -b --baseaddr   Base address of the file to disassemble" << std::endl;
   std::cout << "  -a --address    Address of extra entry points to recursively disassemble (multiple)" << std::endl;
   std::cout << "  -h --help       Show this help" << std::endl;
   std::cout << "  -o --opcodes    Show opcodes in the disassembly" << std::endl;
   std::cout << "  -l --location   Show locations / memory address of instructions and data in the listing" << std::endl;
   std::cout << std::endl;
}

int main(int argc, char* argv[])
{
   struct option long_options[] = {
      { "file",     required_argument, 0, 'f'},
      { "baseaddr", required_argument, 0, 'b'},
      { "addr",     required_argument, 0, 'a'},
      { "help",     no_argument,       0, 'h'},
      { "opcodes",  no_argument,       0, 'o'},
      { "location", no_argument,       0, 'l'},
      { 0,          0,                 0, 0}
   };

   // Default settings
   std::string filename   = "";
   CpuAddress baseAddress = 0x0;
   bool baseAddressDefined = false;
   bool printOpCodes      = false;
   bool printLocations    = false;

   std::vector<CpuAddress> executionEntryPoints;

   int optIndex;

   while(true)
   {
      char optChar = getopt_long(argc, argv, "f:b:a:hol", long_options, &optIndex);

      if (optChar == -1)
      {
         //qDebug() << "Arg parsing complete";
         break;
      }

      switch (optChar)
      {
      case 'f':
         LOG_DEBUG() << "Filename:" << optarg;
         filename = optarg;
         break;

      case 'b':
      {
         baseAddress = Utils::parseUInt16(optarg);
         LOG_DEBUG() << "Base Address:" << addressToString(baseAddress);
         baseAddressDefined = true;
         break;
      }

      case 'a':
      {
         CpuAddress addr = Utils::parseUInt16(optarg);
         LOG_DEBUG() << "Extra Entry Point:" << addressToString(addr);
         executionEntryPoints.push_back(addr);
         break;
      }

      case 'h':
      {
         printUsage(argv[0]);
         return 0;
      }

      case 'o':
      {
         LOG_DEBUG() << "Show op codes in disassembly enabled";
         printOpCodes = true;
         break;
      }

      case 'l':
      {
         LOG_DEBUG() << "Show location information in disassembly enabled";
         printLocations = true;
         break;
      }

      default:
         std::cerr << "Invalid argument.  Use -h or --help to see usage" << std::endl;
      }
   }

   if ( (filename == "") || (!baseAddressDefined) )
   {
      LOG_WARNING() << "Filename and base address are required parameters!";
      printUsage(argv[0]);
      return 1;
   }

   LOG_DEBUG() << "Disassemling " << filename << " at address " << addressToString(baseAddress);

   constructCpuGlobals();

   RomMemory programData("Binary ROM");
   programData.setIntConfigValue("startAddress", baseAddress);
   programData.setStringConfigValue("romFilename", filename);
   programData.resetMemory();

   MemoryController memControl;
   memControl.addNewDevice( (MemoryDev*) &programData);

   Disassembler6502 dis(&memControl);
   dis.includeOpCodes(printOpCodes);
   dis.includeAddress(printLocations);

   for(auto singleEp = executionEntryPoints.begin();
       singleEp != executionEntryPoints.end();
       singleEp++)
   {
      dis.addExtraEntryPoint(*singleEp);
   }

   dis.start(baseAddress);

   dis.printDisassembly();

   return 0;
}

