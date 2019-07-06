#include "Cpu6502.h"
#include "Logger.h"
#include "Utils.h"

#include "DebugServer.h"
#include "MemoryController.h"
#include "MemoryDev.h"


#ifdef TRACE_EXECUTION
   #include "Disassembler6502.h"
#endif

#ifdef CPU_TRACE
   #define CPU_DEBUG    LOG_DEBUG
   #define CPU_WARNING  LOG_WARNING
#else
   #define CPU_DEBUG    if(0) LOG_DEBUG
   #define CPU_WARNING  if(0) LOG_WARNING
#endif

#ifdef ADDR_MODE_TRACE
   #define ADDRMODE_DEBUG   LOG_DEBUG
   #define ADDRMODE_WARNING LOG_WARNING
#else
   #define ADDRMODE_DEBUG    if(0) LOG_DEBUG
   #define ADDRMODE_WARNING  if(0) LOG_WARNING
#endif

Cpu6502::Cpu6502(MemoryController* ctrlr):
   Decoder6502(),
   theRegX(0),
   theRegY(0),
   theAccum(0),
   theStackPtr(0xfd),
   theDebugger(nullptr),
   theDebuggerShutdownFlag(false),
   theRunFlag(true),
   theNumClocks(0),
   theAddrModeExtraClockCycle(0),
   thePageBoundaryCrossedFlag(false)
{
   thePc = 0;

   theMemoryController = ctrlr;

   theStatusReg.theWholeRegister = 0x24;

#ifdef TRACE_EXECUTION
   theDisAss = new Disassembler6502(ctrlr);
   theDisAss->includeOpCodes(true);
   theDisAss->includeAddress(true);

   theTraceFile = fopen("trace.txt", "w");

   if (!theTraceFile)
   {
      LOG_WARNING() << "Error opening trace.txt execution trace file";
   }
   else
   {
      fprintf(theTraceFile, "Execution start\n");
   }

   theNumberOfStepsToTrace = 0xffffffffffffffff;
   theNumberOfStepsExecuted = 0;
#endif
}

Cpu6502::~Cpu6502()
{
   if (theDebugger != nullptr)
   {
      delete theDebugger;
   }

#ifdef TRACE_EXECUTION
   delete theDisAss;

   if (theTraceFile)
   {
      fprintf(theTraceFile, "Execution end\n");
      fclose(theTraceFile);
      theTraceFile = nullptr;
   }
#endif
}

void Cpu6502::enableDebugger(uint16_t portNumber)
{
   if (theDebugger == nullptr)
   {
      CPU_DEBUG() << "Enabling debugger on port " << portNumber;
      theDebugger = new DebugServer(this, portNumber, theMemoryController);
   }
   else
   {
      CPU_WARNING() << "Debugger was already running!";
   }
}

#ifdef TRACE_EXECUTION
void Cpu6502::setStepLimit(uint64_t numSteps)
{
   theNumberOfStepsToTrace = numSteps;
}
#endif

void Cpu6502::exitEmulation()
{
   CPU_DEBUG() << "exit Emulation called";
   theRunFlag = false;
   theDebuggerShutdownFlag = true;
}

void Cpu6502::start()
{
   // Do a bunch of architecutre specific setup stuff here if necessary

   CPU_DEBUG() << "Cpu6502::start called at address " << addressToString(thePc);

   while(true)
   {
      CPU_DEBUG() << "Cpu6502::start start of the loop executing";

      //SDL_Delay(2000);

      if (-1 == decode())
      {
         break;
      }
   }

   CPU_DEBUG() << "Emulator exitting (run flag false)";

}

int Cpu6502::decode()
{
   // Check for the few instances where we can't decode
   // If there is a debugger, do the debugger hook
   if (theDebugger != nullptr)
   {
      theDebugger->debugHook();

      int cc = Decoder6502::decode();
      if (cc == -1)
      {
         // Is the emulator just stopped, or do we really need to shut down
         return (theDebuggerShutdownFlag ? -1 : 0);
      }
      else
      {
         return cc;
      }
   }

   if (!theRunFlag)
   {
      CPU_DEBUG() << "Run flag set to false!";
      return -1;
   }

   return Decoder6502::decode();

}

void Cpu6502::halt()
{
   if (theDebugger != nullptr)
   {
      CPU_DEBUG() << "Emulator halting (debugger attached)";
      theDebugger->emulatorHalt((void*) theDebugger);
   }
   else
   {
      CPU_WARNING() << "Emulator halting (no debugger)";
      theRunFlag = false;
   }

}

void Cpu6502::updatePc(uint8_t bytesIncrement)
{
   // Check clock cycles
   CPU_DEBUG() << "Updating PC";


   thePc += bytesIncrement;


}

void Cpu6502::preHandlerHook(OpCodeInfo* oci)
{
   // This is where sub-classes will do sub-class specific behavior if necessary
   CPU_DEBUG() << "Cpu6502::preHandlerHook called";

   switch(oci->theAddrMode)
   {
   case IMPLIED:
      // There are no operands
      thePageBoundaryCrossedFlag = false;

      ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc) << " is IMPLIED";
      break;

   case IMMEDIATE:
      theOperandVal = theOpCode2;\
      thePageBoundaryCrossedFlag = false;

      ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                       << " is IMMEDIATE, operand is " << Utils::toHex8(theOperandVal);
      break;

   case ZERO_PAGE:
      theOperandAddr = theOpCode2;
      // You can't leave the zeropage for this instruction
      thePageBoundaryCrossedFlag = false;

      ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                       << " is ZERO_PAGE, operand addr is "
                       << addressToString(theOperandAddr);
      break;

   case ZERO_PAGE_X:
      theOperandAddr = theOpCode2;
      theOperandAddr += theRegX;
      theOperandAddr = (theOperandAddr & 0x00ff);

      // You can't leave the zeropage for this instruction
      thePageBoundaryCrossedFlag = false;

      ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                       << " is ZERO_PAGE_X, operand add is "
                       << addressToString(theOperandAddr);
      break;

   case ZERO_PAGE_Y:
      theOperandAddr = theOpCode2;
      theOperandAddr += theRegY;
      theOperandAddr = (theOperandAddr & 0x00ff);

      // You can't leave the zeropage for this instruction
      thePageBoundaryCrossedFlag = false;

      ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                       << " is ZERO_PAGE_Y, operand addr is "
                       << addressToString(theOperandAddr);
      break;

   case ABSOLUTE:
   case INDIRECT:
      theOperandAddr = theOpCode2 + (theOpCode3 << 8);
      thePageBoundaryCrossedFlag = false;

      ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                       << " is " << (oci->theAddrMode == ABSOLUTE ? "ABSOLUTE" : "INDIRECT")
                       << ", operand addr is " << addressToString(theOperandAddr);
      break;

   case ABSOLUTE_X:
      {
         uint16_t opAddr = theOpCode2 + (theOpCode3 << 8);
         theOperandAddr = opAddr + theRegX;

         thePageBoundaryCrossedFlag = ( (opAddr & 0xff00) != (theOperandAddr & 0xff00) );

         ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                          << " is ABSOLUTE_X, operand addr is "
                          << addressToString(theOperandAddr);
         break;
      }

   case ABSOLUTE_Y:
      {
         uint16_t opAddr = theOpCode2 + (theOpCode3 << 8);
         theOperandAddr = opAddr + theRegY;

         thePageBoundaryCrossedFlag = ( (opAddr & 0xff00) != (theOperandAddr & 0xff00) );

         ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                          << " is ABSOLUTE_Y, operand addr is "
                          << addressToString(theOperandAddr);
         break;
      }

   case RELATIVE:
      {
         int16_t theRelativeOperand;
         if (theOpCode2 & 0x80)
         {
            // Sign bit set, operand is -128 to -1
            theRelativeOperand = theOpCode2 ^ 0x00ff;
            theRelativeOperand += 1;
            theRelativeOperand *= -1;
         }
         else
         {
            theRelativeOperand = theOpCode2;
         }

         theOperandAddr = theRelativeOperand + thePc;

         thePageBoundaryCrossedFlag = ( (thePc & 0xff00) != (theOperandAddr & 0xff00) );

         ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                          << " is RELATIVE, operand addr is "
                          << addressToString(theOperandAddr);
         break;
      }

   case INDIRECT_X:
      {
         // Calculate the address of the memory that holds the address location
         // It must be on the zeropage, so we will add X seperately
         uint8_t zeroPageAddressLower = theOpCode2 + theRegX;
         uint8_t zeroPageAddressUpper = zeroPageAddressLower + 1;

         // Now we need to read the address out of that memory
         theOperandAddr = emulatorRead(zeroPageAddressUpper) << 8;
         theOperandAddr += emulatorRead(zeroPageAddressLower);

         thePageBoundaryCrossedFlag = false;

         ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                          << " is INDIRECT_X, operand addr is "
                          << addressToString(theOperandAddr);
         break;
      }

   case INDIRECT_Y:
      {
         // Incase the address rolled over from page 0
         uint16_t zeroPageAddrLowerBytes = theOpCode2;
         uint16_t zeroPageAddrUpperBytes = theOpCode2 + 1;
         zeroPageAddrUpperBytes = (zeroPageAddrUpperBytes & 0x00ff);

         theOperandAddr = (emulatorRead(zeroPageAddrUpperBytes) << 8);
         theOperandAddr += emulatorRead(zeroPageAddrLowerBytes);
         theOperandAddr += theRegY;

         ///@todo I don't know what a rollover page boundary means for this mode...
         thePageBoundaryCrossedFlag = false;

         ADDRMODE_DEBUG() << "ADDRMODE @ " << addressToString(thePc)
                          << " is INDIRECT_Y, operand addr is "
                          << addressToString(theOperandAddr);
         break;
      }

   default:
      LOG_FATAL() << "preHandlerHook unimplemented addressing mode @ " << Utils::toHex16(thePc);
   }

   theAddrModeExtraClockCycle = 0;

#ifdef TRACE_EXECUTION
   std::string disassText = theDisAss->debugListing(thePc, 1);
   disassText.pop_back();  // Remove line ending

   // Lets get the length to 45 characters
   while(disassText.size() < 45)
   {
      disassText.push_back(' ');
   }

   disassText += "A=";
   disassText += Utils::toHex8(theAccum, false);
   disassText += ",  X=";
   disassText += Utils::toHex8(theRegX, false);
   disassText += ",  Y=";
   disassText += Utils::toHex8(theRegY, false);
   disassText += ",  SP=";
   disassText += Utils::toHex8(theStackPtr, false);
   disassText += ",  SR=";
   disassText += Utils::toHex8(theStatusReg.theWholeRegister, false);
   disassText += ",  CLK=";
   disassText += Utils::toHex64(theNumClocks, false);

   fprintf(theTraceFile, "%s\n", disassText.c_str());
#endif
}

int Cpu6502::postHandlerHook(OpCodeInfo* oci)
{
   int clockCycles = oci->theDelayCycles + theAddrModeExtraClockCycle;
   theNumClocks += clockCycles;

#ifdef TRACE_EXECUTION
   theNumberOfStepsExecuted++;

   if (theNumberOfStepsToTrace < theNumberOfStepsExecuted)
   {
      fprintf(theTraceFile, "Hit the step limit of %ld\n", theNumberOfStepsToTrace);
      halt();
   }
   else
   {
      CPU_DEBUG() << "Step target " << theNumberOfStepsToTrace << ", we are at "
                  << theNumberOfStepsExecuted;
   }
#endif

   CPU_DEBUG() << "Clock cycles for this instruction = " << clockCycles;
   return clockCycles;
}

void Cpu6502::getRegisters(uint8_t* regX, uint8_t* regY, uint8_t* accum)
{
   *regX = theRegX;
   *regY = theRegY;
   *accum = theAccum;
}

CpuAddress Cpu6502::getPc()
{
   return thePc;
}

uint8_t Cpu6502::getStackPointer()
{
   return theStackPtr;
}

uint8_t Cpu6502::getStatusReg()
{
   return theStatusReg.theWholeRegister;
}

uint64_t Cpu6502::getInstructionCount()
{
   return theNumClocks;
}

uint8_t Cpu6502::emulatorRead(CpuAddress addr)
{
   if (theDebugger)
   {
      // Call debugger incase user has memory access breakpoint
      theDebugger->debugMemoryAccessHook(addr, false);
   }

   MemoryDev* theMemDev = theMemoryController->getDevice(addr);

   if (theMemDev == nullptr)
   {
      LOG_WARNING() << "Emulator tried to read invalid memory @ " << Utils::toHex16(addr);

      halt();

      return 0xff;
   }
   else
   {
      return theMemDev->read8(addr);
   }
}

void Cpu6502::emulatorWrite(CpuAddress addr, uint8_t val)
{
   if (theDebugger)
   {
      // Call debugger incase user has memory access breakpoint
      theDebugger->debugMemoryAccessHook(addr, true);
   }

   MemoryDev* theMemDev = theMemoryController->getDevice(addr);

   if (theMemDev == nullptr)
   {
      LOG_WARNING() << "Emulator tried to write invalid memory @ " << Utils::toHex16(addr);

      halt();

      return;
   }
   else
   {
      theMemDev->write8(addr, val);
   }
}

/***** HELPER MACROS *****/
#define UPDATE_SIGN_FLAG(regValue) theStatusReg.theSignFlag = (regValue & 0x80 ? 1 : 0)
#define UPDATE_ZERO_FLAG(regValue) theStatusReg.theZeroFlag = (regValue == 0 ? 1 : 0)

#define UPDATE_SZ_FLAGS(regValue)   UPDATE_SIGN_FLAG(regValue); UPDATE_ZERO_FLAG(regValue)

/********************************************/
/************   LOGIC OPERATIONS ************/
/********************************************/

void Cpu6502::handler_and(OpCodeInfo* oci)
{
   CPU_DEBUG() << "AND Handler";

   if (oci->theAddrMode == IMMEDIATE)
   {
      theAccum &= theOperandVal;
   }
   else
   {
      theAccum &= emulatorRead(theOperandAddr);
   }

   UPDATE_SZ_FLAGS(theAccum);

   if (thePageBoundaryCrossedFlag)
   {
      if ( (oci->theAddrMode == ABSOLUTE_X) ||
           (oci->theAddrMode == ABSOLUTE_Y) ||
           (oci->theAddrMode == INDIRECT_Y) )
      {
         theAddrModeExtraClockCycle = 1;
      }
   }
}

void Cpu6502::handler_ora(OpCodeInfo* oci)
{
   CPU_DEBUG() << "ORA Handler";

   if (oci->theAddrMode == IMMEDIATE)
   {
      theAccum |= theOperandVal;
   }
   else
   {
      theAccum |= emulatorRead(theOperandAddr);
   }

   UPDATE_SZ_FLAGS(theAccum);

   if (thePageBoundaryCrossedFlag)
   {
      if ( (oci->theAddrMode == ABSOLUTE_X) ||
           (oci->theAddrMode == ABSOLUTE_Y) ||
           (oci->theAddrMode == INDIRECT_Y) )
      {
         theAddrModeExtraClockCycle = 1;
      }
   }
}

void Cpu6502::handler_eor(OpCodeInfo* oci)
{
   CPU_DEBUG() << "EOR Handler";

   if (oci->theAddrMode == IMMEDIATE)
   {
      theAccum ^= theOperandVal;
   }
   else
   {
      theAccum ^= emulatorRead(theOperandAddr);
   }

   UPDATE_SZ_FLAGS(theAccum);

   if (thePageBoundaryCrossedFlag)
   {
      if ( (oci->theAddrMode == ABSOLUTE_X) ||
           (oci->theAddrMode == ABSOLUTE_Y) ||
           (oci->theAddrMode == INDIRECT_Y) )
      {
         theAddrModeExtraClockCycle = 1;
      }
   }
}

void Cpu6502::handler_rol(OpCodeInfo* oci)
{
   CPU_DEBUG() << "ROL Handler";

   bool setCarryFlag;
   bool setSignFlag;
   bool setZeroFlag;

   bool oldCarryFlag = theStatusReg.theCarryFlag;

   if (oci->theAddrMode == IMPLIED)
   {
      // We are shifting the accumulator
      setCarryFlag = theAccum & 0x80;
      theAccum <<= 1;

      if (oldCarryFlag)
      {
         // We shift in the value of the old carry flag
         theAccum |= 0x01;
      }

      setSignFlag = theAccum & 0x80;
      setZeroFlag = theAccum == 0;
   }
   else
   {
      // We are shifting a byte in memory
      uint8_t val = emulatorRead(theOperandAddr);

      setCarryFlag = val & 0x80;
      val <<= 1;

      if (oldCarryFlag)
      {
         // We shift in the value of the old carry flag
         val |= 0x01;
      }

      emulatorWrite(theOperandAddr, val);

      setSignFlag = val & 0x80;
      setZeroFlag = val == 0;
   }

   theStatusReg.theCarryFlag = (setCarryFlag ? 1 : 0);
   theStatusReg.theSignFlag = (setSignFlag ? 1 : 0);
   theStatusReg.theZeroFlag = (setZeroFlag ? 1 : 0);
}
void Cpu6502::handler_ror(OpCodeInfo* oci)
{
   CPU_DEBUG() << "ROR Handler";

   bool setCarryFlag;
   bool setSignFlag;
   bool setZeroFlag;

   bool oldCarryFlag = theStatusReg.theCarryFlag;

   if (oci->theAddrMode == IMPLIED)
   {
      // We are shifting the accumulator
      setCarryFlag = theAccum & 0x01;
      theAccum >>= 1;

      if (oldCarryFlag)
      {
         // We shift in the value of the old carry flag
         theAccum |= 0x80;
      }

      setSignFlag = theAccum & 0x80;
      setZeroFlag = theAccum == 0;
   }
   else
   {
      // We are shifting a byte in memory
      uint8_t val = emulatorRead(theOperandAddr);

      setCarryFlag = val & 0x01;
      val >>= 1;

      if (oldCarryFlag)
      {
         // We shift in the value of the old carry flag
         val |= 0x80;
      }

      emulatorWrite(theOperandAddr, val);

      setSignFlag = val & 0x80;
      setZeroFlag = val == 0;
   }

   theStatusReg.theCarryFlag = (setCarryFlag ? 1 : 0);
   theStatusReg.theSignFlag = (setSignFlag ? 1 : 0);
   theStatusReg.theZeroFlag = (setZeroFlag ? 1 : 0);
}

void Cpu6502::handler_asl(OpCodeInfo* oci)
{
   CPU_DEBUG() << "ASL Handler";

   bool setCarryFlag;
   bool setSignFlag;
   bool setZeroFlag;

   if (oci->theAddrMode == IMPLIED)
   {
      // We are shifting the accumulator
      setCarryFlag = theAccum & 0x80;
      theAccum <<= 1;

      setSignFlag = theAccum & 0x80;
      setZeroFlag = theAccum == 0;
   }
   else
   {
      // We are shifting a byte in memory
      uint8_t val = emulatorRead(theOperandAddr);

      setCarryFlag = val & 0x80;
      val <<= 1;

      emulatorWrite(theOperandAddr, val);

      setSignFlag = val & 0x80;
      setZeroFlag = val == 0;
   }

   theStatusReg.theCarryFlag = (setCarryFlag ? 1 : 0);
   theStatusReg.theSignFlag = (setSignFlag ? 1 : 0);
   theStatusReg.theZeroFlag = (setZeroFlag ? 1 : 0);
}

void Cpu6502::handler_lsr(OpCodeInfo* oci)
{
   CPU_DEBUG() << "LSR Handler";

   bool setCarryFlag;
   bool setZeroFlag;

   if (oci->theAddrMode == IMPLIED)
   {
      // We are shifting the accumulator
      setCarryFlag = theAccum & 0x01;
      theAccum >>= 1;

      setZeroFlag = theAccum == 0;
   }
   else
   {
      // We are shifting a byte in memory
      uint8_t val = emulatorRead(theOperandAddr);

      setCarryFlag = val & 0x01;
      val >>= 1;

      emulatorWrite(theOperandAddr, val);

      setZeroFlag = val == 0;
   }

   theStatusReg.theCarryFlag = (setCarryFlag ? 1 : 0);
   theStatusReg.theSignFlag = 0;
   theStatusReg.theZeroFlag = (setZeroFlag ? 1 : 0);
}

/********************************************/
/***   LOAD / STORE / TRANSFER OPERATIONS ***/
/********************************************/

void Cpu6502::handler_lda(OpCodeInfo* oci)
{
   CPU_DEBUG() << "LDA Handler";

   if (oci->theAddrMode == IMMEDIATE)
   {
      theAccum = theOperandVal;
   }
   else
   {
      // We need to read the actual value to load from memory
      theAccum = emulatorRead(theOperandAddr);
   }

   UPDATE_SZ_FLAGS(theAccum);

   if (thePageBoundaryCrossedFlag)
   {
      if ( (oci->theAddrMode == ABSOLUTE_X) ||
           (oci->theAddrMode == ABSOLUTE_Y) ||
           (oci->theAddrMode == INDIRECT_Y) )
      {
         theAddrModeExtraClockCycle = 1;
      }
   }
}

void Cpu6502::handler_ldx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "LDX Handler";
   if (oci->theAddrMode == IMMEDIATE)
      theRegX = theOperandVal;
   else
   {
      // We need to read the actual value to load from memory
      theRegX = emulatorRead(theOperandAddr);
   }

   UPDATE_SZ_FLAGS(theRegX);

   if ( (oci->theAddrMode == ABSOLUTE_Y) && thePageBoundaryCrossedFlag)
   {
      theAddrModeExtraClockCycle = 1;
   }
}

void Cpu6502::handler_ldy(OpCodeInfo* oci)
{
   CPU_DEBUG() << "LDY Handler";

   if (oci->theAddrMode == IMMEDIATE)
   {
      theRegY = theOperandVal;
   }
   else
   {
      theRegY = emulatorRead(theOperandAddr);
   }

   UPDATE_SZ_FLAGS(theRegY);

   if ( (oci->theAddrMode == ABSOLUTE_X) && thePageBoundaryCrossedFlag)
   {
      theAddrModeExtraClockCycle = 1;
   }
}

void Cpu6502::handler_sta(OpCodeInfo* oci)
{
   CPU_DEBUG() << "STA Handler";

   emulatorWrite(theOperandAddr, theAccum);
}

void Cpu6502::handler_stx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "STX Handler";

   emulatorWrite(theOperandAddr, theRegX);
}

void Cpu6502::handler_sty(OpCodeInfo* oci)
{
   CPU_DEBUG() << "STY Handler";

   emulatorWrite(theOperandAddr, theRegY);
}

void Cpu6502::handler_tax(OpCodeInfo* oci)
{
   CPU_DEBUG() << "TAX Handler";
   theRegX = theAccum;
   UPDATE_SZ_FLAGS(theRegX);
}

void Cpu6502::handler_txa(OpCodeInfo* oci)
{
   CPU_DEBUG() << "TXA Handler";
   theAccum = theRegX;
   UPDATE_SZ_FLAGS(theAccum);
}

void Cpu6502::handler_tay(OpCodeInfo* oci)
{
   CPU_DEBUG() << "TAY Handler";
   theRegY = theAccum;
   UPDATE_SZ_FLAGS(theRegY);
}

void Cpu6502::handler_tya(OpCodeInfo* oci)
{
   CPU_DEBUG() << "TYA Handler";
   theAccum = theRegY;
   UPDATE_SZ_FLAGS(theAccum);
}

void Cpu6502::handler_tsx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "TSX Handler";
   theRegX = theStackPtr;
   UPDATE_SZ_FLAGS(theRegX);
}

void Cpu6502::handler_txs(OpCodeInfo* oci)
{
   CPU_DEBUG() << "TXS Handler";
   theStackPtr = theRegX;
}

/********************************************/
/************* MATH OPERATIONS **************/
/********************************************/

void Cpu6502::handler_inc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "INC Handler";

   uint8_t val = emulatorRead(theOperandAddr);

   val++;

   emulatorWrite(theOperandAddr, val);

   UPDATE_SZ_FLAGS(val);
}

void Cpu6502::handler_inx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "INX Handler";
   theRegX++;
   UPDATE_SZ_FLAGS(theRegX);
}

void Cpu6502::handler_iny(OpCodeInfo* oci)
{
   CPU_DEBUG() << "INY Handler";
   theRegY++;
   UPDATE_SZ_FLAGS(theRegY);
}

void Cpu6502::handler_dec(OpCodeInfo* oci)
{
   CPU_DEBUG() << "DEC Handler";

   uint8_t val = emulatorRead(theOperandAddr);

   val--;

   emulatorWrite(theOperandAddr, val);

   UPDATE_SZ_FLAGS(val);
}

void Cpu6502::handler_dex(OpCodeInfo* oci)
{
   CPU_DEBUG() << "DEX Handler";
   theRegX--;
   UPDATE_SZ_FLAGS(theRegX);
}

void Cpu6502::handler_dey(OpCodeInfo* oci)
{
   CPU_DEBUG() << "DEY Handler";
   theRegY--;
   UPDATE_SZ_FLAGS(theRegY);
}

void Cpu6502::addition_operation(uint8_t operandValue, OpCodeInfo* oci)
{
   uint16_t sum = theAccum + operandValue + theStatusReg.theCarryFlag;
   //uint8_t oldAccum = theAccum;

   // Copied the code from Mesen emulator on the overflow flag cause I basically have no idea
   if(~(theAccum ^ operandValue) & (theAccum ^ sum) & 0x80)
   {
      theStatusReg.theOverflowFlag = 1;
   }
   else
   {
      theStatusReg.theOverflowFlag = 0;
   }

   theAccum = sum & 0xff;
   UPDATE_SZ_FLAGS(theAccum);

   theStatusReg.theCarryFlag = (sum & 0xff00 ? 1 : 0);

   // CPU_DEBUG() << "Addition inputs: operand=" << Utils::toHex8(operandValue)
   //             << ", accum=" << Utils::toHex8(oldAccum)
   //             << ", cf=" << Utils::toHex8(theStatusReg.theCarryFlag);
   // CPU_DEBUG() << "Addition output: sum=" << Utils::toHex16(sum)
   //             << ", accum=" << Utils::toHex8(theAccum);

   if (thePageBoundaryCrossedFlag)
   {
      if ( (oci->theAddrMode == ABSOLUTE_X) ||
           (oci->theAddrMode == ABSOLUTE_Y) ||
           (oci->theAddrMode == INDIRECT_Y) )
      {
         theAddrModeExtraClockCycle = 1;
      }
   }
}

void Cpu6502::handler_adc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "ADC Handler";

   if (oci->theAddrMode == IMMEDIATE)
   {
      addition_operation(theOperandVal, oci);
   }
   else
   {
      addition_operation(emulatorRead(theOperandAddr), oci);
   }
}

void Cpu6502::handler_sbc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "SBC Handler";

   // Strategy.  Just invert the operand and call addition.  Crazy, right...
   if (oci->theAddrMode == IMMEDIATE)
   {
      // 1s complement
      addition_operation(0xff ^ theOperandVal, oci);
   }
   else
   {
      // 1s complement
      addition_operation(0xff ^ emulatorRead(theOperandAddr), oci);
   }
}

/********************************************/
/************* FLAG OPERATIONS **************/
/********************************************/

void Cpu6502::handler_clc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "CLC Handler - Clears carry flag";
   theStatusReg.theCarryFlag = 0;
}

void Cpu6502::handler_sec(OpCodeInfo* oci)
{
   CPU_DEBUG() << "SEC Handler - Sets carry flag";
   theStatusReg.theCarryFlag = 1;
}

void Cpu6502::handler_cli(OpCodeInfo* oci)
{
   CPU_DEBUG() << " CLI Handler - Clears interrupt disable flag";
   theStatusReg.theInterruptFlag = 0;
}

void Cpu6502::handler_sei(OpCodeInfo* oci)
{
   CPU_DEBUG() << "SEI Handler = Sets interrupt disable flag";
   theStatusReg.theInterruptFlag = 1;
}

void Cpu6502::handler_cld(OpCodeInfo* oci)
{
   CPU_DEBUG() << "CLD Handler - Clears the decimal mode flag";
   theStatusReg.theBCDFlag = 0;
}

void Cpu6502::handler_sed(OpCodeInfo* oci)
{
   CPU_DEBUG() << "SED Handler - Sets the decimal mode flag";
   theStatusReg.theBCDFlag = 1;
}

void Cpu6502::handler_clv(OpCodeInfo* oci)
{
   CPU_DEBUG() << "CLV Handler - Clears the overflow flag";
   theStatusReg.theOverflowFlag = 0;
}

/********************************************/
/************ COMPARE OPERATIONS ************/
/********************************************/

void Cpu6502::comparison_operation(uint8_t regValue, uint8_t operandValue)
{
   if (regValue == operandValue)
   {
      CPU_DEBUG() << "Comparison Operation: " << Utils::toHex8(operandValue) << "== regValue ("
                  << Utils::toHex8(regValue) << ")";
      theStatusReg.theCarryFlag = 1;
      theStatusReg.theZeroFlag = 1;
      theStatusReg.theSignFlag = 0;
   }
   else
   {
      // They are not the same
      theStatusReg.theZeroFlag = 0;
      theStatusReg.theCarryFlag = (regValue > operandValue ? 1 : 0);

      // Lets do some subtraction, 8 bit style
      operandValue ^= 0xff;
      uint16_t sum = operandValue + regValue + 1;
      operandValue = sum & 0xff;

      CPU_DEBUG() << "Comparison Operation: After subtraction " << Utils::toHex8(operandValue);

      UPDATE_SIGN_FLAG(operandValue);
   }
}

void Cpu6502::handler_cmp(OpCodeInfo* oci)
{
   uint8_t val;
   if (oci->theAddrMode == IMMEDIATE)
   {
      val = theOperandVal;
   }
   else
   {
      val = emulatorRead(theOperandAddr);
   }

   comparison_operation(theAccum, val);

   if ( (oci->theAddrMode == ABSOLUTE_X) ||
        (oci->theAddrMode == ABSOLUTE_Y) ||
        (oci->theAddrMode == INDIRECT_Y) )
   {
      theAddrModeExtraClockCycle = 1;
   }
}

void Cpu6502::handler_cpx(OpCodeInfo* oci)
{
   uint8_t val;
   if (oci->theAddrMode == IMMEDIATE)
   {
      val = theOperandVal;
   }
   else
   {
      val = emulatorRead(theOperandAddr);
   }

   comparison_operation(theRegX, val);
}

void Cpu6502::handler_cpy(OpCodeInfo* oci)
{
   uint8_t val;
   if (oci->theAddrMode == IMMEDIATE)
   {
      val = theOperandVal;
   }
   else
   {
      val = emulatorRead(theOperandAddr);
   }

   comparison_operation(theRegY, val);
}

void Cpu6502::handler_bit(OpCodeInfo* oci)
{
   uint8_t opVal = emulatorRead(theOperandAddr);
   uint8_t val= opVal & theAccum;

   theStatusReg.theZeroFlag = (val == 0 ? 1 : 0);

   // This was a bit confusing at first, but just use the bits 6 and 7 directly from opVal
   theStatusReg.theOverflowFlag = ( opVal & 0x40 ? 1 : 0);
   theStatusReg.theSignFlag = (opVal & 0x80 ? 1 : 0);

   CPU_DEBUG() << "BIT Handler - *op & accum = " << Utils::toHex8(opVal) << " & "
               << Utils::toHex8(theAccum) << " = " << Utils::toHex8(val);
}

/********************************************/
/************* BRANCH OPERATIONS ************/
/********************************************/

void Cpu6502::handler_jmp(OpCodeInfo* oci)
{
   CPU_DEBUG() << "JMP Handler";

   if (oci->theAddrMode == ABSOLUTE)
   {
      // The address is in the opcode directly

      // We set PC to the address minus length of our own opcode since updatePc will
      // increment PC after we run
      thePc = theOperandAddr - oci->theNumBytes;
   }
   else
   {
      // The address in the memory pointed to by the opcode address
      uint8_t lowerAddress = emulatorRead(theOperandAddr);

      // The upper byte of the address can't cross a page boundary (it will wrap to 0x00 on same page
      CpuAddress upperByteAddress = (theOperandAddr & 0xff00) + ( (theOperandAddr + 1) & 0x00ff);
      uint8_t upperAddress = emulatorRead(upperByteAddress);

      uint16_t targetAddr = upperAddress << 8;
      targetAddr += lowerAddress;

      thePc = targetAddr - oci->theNumBytes;
   }

   // No flags are affected
}

void Cpu6502::handler_jsr(OpCodeInfo* oci)
{
   CPU_DEBUG() << "JSR Handler";

   // due to behavior of rts on 6502, the returns address has 1 byte subtracted
   uint16_t addressToPush = thePc + oci->theNumBytes - 1;

   uint8_t addressUpper = ((addressToPush >> 8) & 0x00ff);
   uint8_t addressLower = addressToPush & 0x00ff;

   if (theStackPtr == 0x0100)
   {
      // We are going to overflow the stack, wtg n00b
      LOG_WARNING() << "6502 stack overflow @ addr " << addressToString(thePc)
                    << " from a JSR instruction";
   }

   emulatorWrite(0x0100 + theStackPtr, addressUpper);
   theStackPtr--;
   emulatorWrite(0x0100 + theStackPtr, addressLower);
   theStackPtr--;

   // JSR is always an ABSOLUTE addressing mode instruction
   thePc = theOperandAddr - oci->theNumBytes;
}

void Cpu6502::handler_rts(OpCodeInfo* oci)
{
   CPU_DEBUG() << "RTS Handler";

   // Retrieve the new PC from the stack, lower address first, uppper address last

   // Like my emulator, the real 6502 increments PC after this instruction to get
   // the real PC for the next instruction
   theStackPtr++;
   uint8_t lowerAddress = emulatorRead(0x0100 + theStackPtr);
   theStackPtr++;
   uint8_t upperAddress = emulatorRead(0x0100 + theStackPtr);

   thePc = upperAddress << 8;
   thePc += lowerAddress;
}

void Cpu6502::handler_rti(OpCodeInfo* oci)
{
   theStackPtr++;
   uint8_t srVal = emulatorRead(0x0100 + theStackPtr);

   // Don't touch bit 4 and 5 of status register
   uint8_t unaffectedBits = theStatusReg.theWholeRegister & 0x30;
   theStatusReg.theWholeRegister = srVal & 0xBF;
   theStatusReg.theWholeRegister = srVal | unaffectedBits;

   CPU_DEBUG() << "RTI Handler - Popped SR (" << Utils::toHex8(srVal) << ") from stack";

   theStackPtr++;
   uint8_t lowerAddress = emulatorRead(0x0100 + theStackPtr);
   theStackPtr++;
   uint8_t upperAddress = emulatorRead(0x0100 + theStackPtr);

   thePc = upperAddress << 8;
   thePc += lowerAddress;

   CPU_DEBUG() << "RTI Handler - Popped PC (" << addressToString(thePc) << ") from stack, now SP ="
               << Utils::toHex8(theStackPtr);

   thePc -= oci->theNumBytes;
}



void Cpu6502::handler_beq(OpCodeInfo* oci)
{
   if (theStatusReg.theZeroFlag == 1)
   {
      CPU_DEBUG() << "BEQ Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BEQ Handler - Branch not taken";
   }
}

void Cpu6502::handler_bne(OpCodeInfo* oci)
{
   if (theStatusReg.theZeroFlag == 0)
   {
      CPU_DEBUG() << "BNE Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BNE Handler - Branch not taken";
   }
}

void Cpu6502::handler_bpl(OpCodeInfo* oci)
{
   if (theStatusReg.theSignFlag == 0)
   {
      CPU_DEBUG() << "BPL Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BPL Handler - Branch not taken";
   }
}

void Cpu6502::handler_bmi(OpCodeInfo* oci)
{
   if (theStatusReg.theSignFlag == 1)
   {
      CPU_DEBUG() << "BMI Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BMI Handler - Branch not taken";
   }
}

void Cpu6502::handler_bvc(OpCodeInfo* oci)
{
   if (theStatusReg.theOverflowFlag == 0)
   {
      CPU_DEBUG() << "BVC Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BVC Handler - Branch not taken";
   }
}

void Cpu6502::handler_bvs(OpCodeInfo* oci)
{
   if (theStatusReg.theOverflowFlag == 1)
   {
      CPU_DEBUG() << "BVS Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BVS Handler - Branch not taken";
   }
}

void Cpu6502::handler_bcc(OpCodeInfo* oci)
{
   if (theStatusReg.theCarryFlag == 0)
   {
      CPU_DEBUG() << "BCC Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BCC Handler - Branch not taken";
   }
}

void Cpu6502::handler_bcs(OpCodeInfo* oci)
{
   if (theStatusReg.theCarryFlag == 1)
   {
      CPU_DEBUG() << "BCS Handler - Branch taken to " << addressToString(theOperandAddr);

      // Delay of +1 if branch taken, +2 if crossing boundary
      theAddrModeExtraClockCycle = ( thePageBoundaryCrossedFlag ? 2 : 1);

      thePc = theOperandAddr;
   }
   else
   {
      CPU_DEBUG() << "BCS Handler - Branch not taken";
   }
}

/********************************************/
/************* STACK OPERATIONS *************/
/********************************************/

void Cpu6502::handler_pha(OpCodeInfo* oci)
{
   emulatorWrite(0x0100 + theStackPtr, theAccum);
   theStackPtr--;

   CPU_DEBUG() << "PHA Handler - Pushed A (" << Utils::toHex8(theAccum) << ") onto stack, not SP="
               << Utils::toHex8(theStackPtr);
}

void Cpu6502::handler_pla(OpCodeInfo* oci)
{
   theStackPtr++;
   theAccum = emulatorRead(0x0100 + theStackPtr);


   CPU_DEBUG() << "PHA Handler - Popped A (" << Utils::toHex8(theAccum) << ") from stack, now SP="
               << Utils::toHex8(theStackPtr);

   UPDATE_SZ_FLAGS(theAccum);
}

void Cpu6502::handler_php(OpCodeInfo* oci)
{
   uint8_t valueToPush;

   // Set bits 4 and 5 before pushing
   // https://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
   valueToPush = theStatusReg.theWholeRegister | 0x30;

   emulatorWrite(0x0100 + theStackPtr, valueToPush);
   theStackPtr--;

   CPU_DEBUG() << "PHP Handler - Pushed SR (" << Utils::toHex8(theStatusReg.theWholeRegister)
               << ") onto stack, not SP=" << Utils::toHex8(theStackPtr);
}

void Cpu6502::handler_plp(OpCodeInfo* oci)
{
   theStackPtr++;

   // When pulling from the stack, we should ignore bits 4 and 5

   uint8_t ignoredBits = theStatusReg.theWholeRegister & 0x30;
   theStatusReg.theWholeRegister = emulatorRead(0x0100 + theStackPtr) & 0xCF;
   theStatusReg.theWholeRegister |= ignoredBits;

   CPU_DEBUG() << "PLP Handler - Popped SR (" << Utils::toHex8(theStatusReg.theWholeRegister)
               << ") from stack, now SP=" << Utils::toHex8(theStackPtr);
}

/********************************************/
/************* OTHER OPERATIONS *************/
/********************************************/

void Cpu6502::handler_anc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for anc";
}

void Cpu6502::handler_arr(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for arr";
}

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_rra(OpCodeInfo* oci)
{

   bool setCarryFlag;
   bool oldCarryFlag = theStatusReg.theCarryFlag;

   // We are shifting a byte in memory
   uint8_t val = emulatorRead(theOperandAddr);

   setCarryFlag = val & 0x01;
   val >>= 1;

   if (oldCarryFlag)
   {
      // We shift in the value of the old carry flag
      val |= 0x80;
   }

   theStatusReg.theCarryFlag = (setCarryFlag ? 1 : 0);

   emulatorWrite(theOperandAddr, val);

   addition_operation(val, oci);

   CPU_DEBUG() << "RRA Handler: Memory @" << addressToString(theOperandAddr) << "="
               << Utils::toHex8(val) << "after rotate right, adding with A ="
               << Utils::toHex8(theAccum);
}
#else
void Cpu6502::handler_rra(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for rra - UNOFFICIAL 6502 OP CODE";
}
#endif

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_sax(OpCodeInfo* oci)
{
   //AND X and Accum registers, store in memory
   uint8_t val = theRegX & theAccum;

   CPU_DEBUG() << "SAX Handler: X & A = " << Utils::toHex8(val);
   emulatorWrite(theOperandAddr, val);
}
#else
void Cpu6502::handler_sax(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sax - UNOFFICIAL 6502 OP CODE";
}
#endif

void Cpu6502::handler_alr(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for alr";
}

void Cpu6502::handler_ahx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for ahx";
}

void Cpu6502::handler_tas(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for tas";
}

void Cpu6502::handler_xaa(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for xaa";
}

void Cpu6502::handler_axs(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for axs";
}

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_rla(OpCodeInfo* oci)
{

   uint8_t val = emulatorRead(theOperandAddr);

   int carryFlag = (val >> 7) & 0x01;

   // Rotate left
   val <<= 1;
   val |= theStatusReg.theCarryFlag;

   emulatorWrite(theOperandAddr, val);

   CPU_DEBUG() << "RLA Handler: Memory @" << addressToString(theOperandAddr) << "="
               << Utils::toHex8(val) << "after rotate left, and-ing with A ="
               << Utils::toHex8(theAccum);

   theAccum &= val;

   theStatusReg.theCarryFlag = carryFlag;
   UPDATE_SZ_FLAGS(theAccum);
}
#else
void Cpu6502::handler_rla(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for rla - UNOFFICIAL 6502 OP CODE";
}
#endif

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_lax(OpCodeInfo* oci)
{
   // Load accumulator and X register with memory.

   // We need to read the actual value to load from memory
   theRegX = emulatorRead(theOperandAddr);
   theAccum = theRegX;

   CPU_DEBUG() << "LAX Handler: Loaded" << Utils::toHex8(theRegX) << " into A and X registers";

   UPDATE_SZ_FLAGS(theRegX);

   if (thePageBoundaryCrossedFlag)
   {
      if ( (oci->theAddrMode == ABSOLUTE_Y) ||
           (oci->theAddrMode == INDIRECT_Y) )
      {
        theAddrModeExtraClockCycle = 1;
      }
   }

}
#else
void Cpu6502::handler_lax(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for lax - UNOFFICIAL 6502 OP CODE";
}
#endif

void Cpu6502::handler_kil(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for kil";
}

void Cpu6502::handler_nop(OpCodeInfo* oci)
{
   CPU_DEBUG() << "NOP Handler";
}

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_dcp(OpCodeInfo* oci)
{
   // Equivalent to DEC followed by a CMP operation

   uint8_t val = emulatorRead(theOperandAddr) - 1;
   emulatorWrite(theOperandAddr, val);

   CPU_DEBUG() << "DCP handler: Mem @" << addressToString(theOperandAddr) << "=" << Utils::toHex8(val)
               << "after decrement, compare to A =" << Utils::toHex8(theAccum);

   comparison_operation(theAccum, val);
}
#else
void Cpu6502::handler_dcp(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for dcp - UNOFFICIAL 6502 OP CODE";
}
#endif

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_sre(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sre";

   uint8_t val = emulatorRead(theOperandAddr);

   int carryFlag = val & 0x01;

   // Shift right and store in memory
   val >>= 1;
   emulatorWrite(theOperandAddr, val);

   CPU_DEBUG() << "SRE Handler: Memory @" << addressToString(theOperandAddr) << "="
               << Utils::toHex8(val) << "after right shift, xoring with A ="
               << Utils::toHex8(theAccum);

   theAccum = theAccum ^ val;

   theStatusReg.theCarryFlag = carryFlag;
   UPDATE_SZ_FLAGS(theAccum);


}
#else
void Cpu6502::handler_sre(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sre - UNOFFICIAL 6502 OP CODE";
}
#endif
void Cpu6502::handler_shx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for shx";
}

void Cpu6502::handler_shy(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for shy";
}

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_slo(OpCodeInfo* oci)
{
   // Equivalent to ASL followed by ORA

   uint8_t val = emulatorRead(theOperandAddr);

   int carryFlag = (val >> 7) & 0x01;
   theStatusReg.theCarryFlag = carryFlag;

   val <<= 1;
   emulatorWrite(theOperandAddr, val);

   CPU_DEBUG() << "SLO Handler: Memory @" << addressToString(theOperandAddr) << "="
               << Utils::toHex8(val) << "after shift left, OR-ing with A ="
               << Utils::toHex8(theAccum);

   theAccum |= val;

   UPDATE_SZ_FLAGS(theAccum);
}
#else
void Cpu6502::handler_slo(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for slo - UNOFFICIAL 6502 OP CODE";
}
#endif
void Cpu6502::handler_las(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for las";
}

#ifdef UNOFFICIAL_NES_OPCODE_SUPPORT
void Cpu6502::handler_isc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for isc";

   uint8_t val = emulatorRead(theOperandAddr) + 1;
   emulatorWrite(theOperandAddr, val);

   CPU_DEBUG() << "ISC Handler: Memory @" << addressToString(theOperandAddr) << "="
               << Utils::toHex8(val) << " after inc, now sbc with A =" << Utils::toHex8(theAccum);

   addition_operation(0xff ^ val, oci);

   // Addition operation will try to add
   theAddrModeExtraClockCycle = 0;
}
#else
void Cpu6502::handler_isc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for isc - UNOFFICIAL 6502 OP CODE";
}
#endif

void Cpu6502::handler_brk(OpCodeInfo* oci)
{
   CPU_DEBUG() << "BRK Handler";

   // due to behavior of rts on 6502, the returns address has 1 byte subtracted
   uint16_t addressToPush = thePc + oci->theNumBytes - 1;

   uint8_t addressUpper = ((addressToPush >> 8) & 0x00ff);
   uint8_t addressLower = addressToPush & 0x00ff;

   if (theStackPtr == 0x0101)
   {
      // We are going to overflow the stack, wtg n00b
      LOG_WARNING() << "6502 stack overflow @ addr " << addressToString(thePc)
                    << " from a BRK instruction";
   }

   emulatorWrite(0x0100 + theStackPtr, addressUpper);
   theStackPtr--;
   emulatorWrite(0x0100 + theStackPtr, addressLower);
   theStackPtr--;

   uint8_t valueToPush;

   // Set bits 4 and 5 before pushing
   // https://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
   valueToPush = theStatusReg.theWholeRegister | 0x30;

   emulatorWrite(0x0100 + theStackPtr, valueToPush);

   CPU_DEBUG() << "BRK pushed " << Utils::toHex8(addressUpper) << ", " << Utils::toHex8(addressLower)
               << ", " << Utils::toHex8(theStatusReg.theWholeRegister);

   theStatusReg.theBreakpointFlag = 1;

   // This likely needs to be modified because the decoder is going to increment PC after this
   thePc = (emulatorRead(0xffff) << 8) + emulatorRead(0xfffe);
}







