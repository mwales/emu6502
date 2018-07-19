#include <iostream>

#include "../Cpu6502Defines.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Hello World Test", "[HelloWorld]")
{
   int i = 2;
   i += 4;

   REQUIRE(i == 6);

   std::cout << "Peaches!" << std::endl;
};

TEST_CASE("Verifying status flags", "[cpu]")
{
   uint8_t sampleValue;

   StatusReg* statusReg;
   statusReg = (StatusReg*) &sampleValue;

   REQUIRE(sizeof(StatusReg) == 1);

   sampleValue = 0;
   statusReg->theSignFlag = 1;
   REQUIRE(sampleValue == FLG_NEG);

   sampleValue = 0;
   statusReg->theOverflowFlag = 1;
   REQUIRE(sampleValue == FLG_OVFL);

   sampleValue = 0;
   statusReg->theNotUsedFlag = 1;
   REQUIRE(sampleValue == 0x20);

   sampleValue = 0;
   statusReg->theBreakpointFlag = 1;
   REQUIRE(sampleValue == FLG_BKPT);

   sampleValue = 0;
   statusReg->theBCDFlag = 1;
   REQUIRE(sampleValue == FLG_DECI);

   sampleValue = 0;
   statusReg->theInterruptFlag = 1;
   REQUIRE(sampleValue == FLG_INTD);

   sampleValue = 0;
   statusReg->theZeroFlag = 1;
   REQUIRE(sampleValue == FLG_ZERO);

   sampleValue = 0;
   statusReg->theCarryFlag = 1;
   REQUIRE(sampleValue == FLG_CARY);

};




