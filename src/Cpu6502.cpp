#include "Cpu6502.h"
#include "Logger.h"
#include "Utils.h"

#include "DebugServer.h"
#include "MemoryController.h"
#include "MemoryDev.h"

#define CPU_TRACE
#define ADDR_MODE_TRACE

#ifdef CPU_TRACE
   #define CPU_DEBUG    LOG_DEBUG
   #define CPU_WARNING  LOG_WARNING
#else
   #define CPU_DEBUG    if(0) LOG_DEBUG
   #define CPU_WARNING  if(0) CPU_WARNING LOG_WARNING
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
   theStackPtr(0xff),
   thePc(0),
   theDebugger(nullptr),
   theRunFlag(true),
   theNumClocks(0),
   theAddrModeExtraClockCycle(0),
   thePageBoundaryCrossedFlag(false)
{
   theMemoryController = ctrlr;

   memset(&theStatusReg, 0, sizeof(StatusReg));
}

Cpu6502::~Cpu6502()
{
   if (theDebugger != nullptr)
   {
      delete theDebugger;
   }
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

void Cpu6502::exitEmulation()
{
   theRunFlag = false;
}

void Cpu6502::start(CpuAddress address)
{
   // Do a bunch of architecutre specific setup stuff here if necessary

   thePc = address;

   while(true)
   {
      CPU_DEBUG() << "Cpu6502::start start of the loop executing";

      // If there is a debugger, do the debugger hook
      if (theDebugger != nullptr)
      {
         theDebugger->debugHook();
      }

      if (!theRunFlag)
         break;

      //SDL_Delay(2000);

      decode(thePc);
   }

   CPU_DEBUG() << "Emulator exitting (run flag false)";

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

         theOperandAddr += theRelativeOperand;

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
}

void Cpu6502::postHandlerHook(OpCodeInfo* oci)
{
   theNumClocks += oci->theDelayCycles;
   theNumClocks += theAddrModeExtraClockCycle;
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
   uint8_t retVal;
   memcpy(&retVal, &theStatusReg, sizeof(StatusReg));
   return retVal;
}

uint64_t Cpu6502::getInstructionCount()
{
   return theNumClocks;
}

uint8_t Cpu6502::emulatorRead(CpuAddress addr)
{
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


/***   CREATING HANDLER BLANK FUNCTIONS ***/
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

   if (theAccum == 0)
      theStatusReg.theZeroFlag = 1;

   if (theAccum & 0x80)
      theStatusReg.theSignFlag = 1;

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

void Cpu6502::handler_bvs(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bvs";
}

void Cpu6502::handler_sec(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sec";
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

void Cpu6502::handler_pla(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for pla";
}

void Cpu6502::handler_anc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for anc";
}

void Cpu6502::handler_rti(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for rti";
}

void Cpu6502::handler_arr(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for arr";
}

void Cpu6502::handler_rra(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for rra";
}

void Cpu6502::handler_bvc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bvc";
}

void Cpu6502::handler_sax(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sax";
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

void Cpu6502::handler_inx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for inx";
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

   if (theRegX == 0)
      theStatusReg.theZeroFlag = 1;

   if (theRegX & 0x80)
      theStatusReg.theSignFlag = 1;

   if ( (oci->theAddrMode == ABSOLUTE_Y) && thePageBoundaryCrossedFlag)
   {
      theAddrModeExtraClockCycle = 1;
   }
}

void Cpu6502::handler_alr(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for alr";
}

void Cpu6502::handler_ahx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for ahx";
}

void Cpu6502::handler_sei(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sei";
}

void Cpu6502::handler_iny(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for iny";
}

void Cpu6502::handler_inc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "INC Handler";

   uint8_t val = emulatorRead(theOperandAddr);

   val++;

   emulatorWrite(theOperandAddr, val);

   if (val == 0)
      theStatusReg.theZeroFlag = 1;

   if (val & 0x80)
      theStatusReg.theSignFlag = 1;
}

void Cpu6502::handler_cli(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for cli";
}

void Cpu6502::handler_beq(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for beq";
}

void Cpu6502::handler_cpy(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for cpy";
}

void Cpu6502::handler_cld(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for cld";
}

void Cpu6502::handler_txs(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for txs";
}

void Cpu6502::handler_tas(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for tas";
}

void Cpu6502::handler_clc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for clc";
}

void Cpu6502::handler_adc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for adc";

   uint16_t val = (uint16_t) theAccum + (uint16_t) theOperandVal;

   if (val > 0xff)
   {
      // We carried!
      theStatusReg.theCarryFlag = 1;
   }

   theAccum = val & 0xff;
}

void Cpu6502::handler_tsx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for tsx";
}

void Cpu6502::handler_xaa(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for xaa";
}

void Cpu6502::handler_clv(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for clv";
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
      uint8_t upperAddress = emulatorRead(theOperandAddr + 1);

      uint16_t targetAddr = upperAddress << 8;
      targetAddr += lowerAddress;

      thePc = targetAddr - oci->theNumBytes;
   }

   // No flags are affected
}

void Cpu6502::handler_bne(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bne";
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

   if (theRegY == 0)
      theStatusReg.theZeroFlag = 1;

   if (theRegY & 0x80)
      theStatusReg.theSignFlag = 1;

   if ( (oci->theAddrMode == ABSOLUTE_X) && thePageBoundaryCrossedFlag)
   {
      theAddrModeExtraClockCycle = 1;
   }
}

void Cpu6502::handler_axs(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for axs";
}

void Cpu6502::handler_plp(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for plp";
}

void Cpu6502::handler_tax(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for tax";
}

void Cpu6502::handler_pha(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for pha";
}

void Cpu6502::handler_bmi(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bmi";
}

void Cpu6502::handler_rla(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for rla";
}

void Cpu6502::handler_tya(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for tya";
}

void Cpu6502::handler_tay(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for tay";
}

void Cpu6502::handler_sbc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sbc";
}

void Cpu6502::handler_lax(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for lax";
}

void Cpu6502::handler_txa(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for txa";
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

void Cpu6502::handler_kil(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for kil";
}

void Cpu6502::handler_bit(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bit";
}

void Cpu6502::handler_php(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for php";
}

void Cpu6502::handler_nop(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for nop";
}

void Cpu6502::handler_dcp(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for dcp";
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

   if (theAccum == 0)
      theStatusReg.theZeroFlag = 1;

   if (theAccum & 0x80)
      theStatusReg.theSignFlag = 1;

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

void Cpu6502::handler_dex(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for dex";
}

void Cpu6502::handler_dey(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for dey";
}

void Cpu6502::handler_dec(OpCodeInfo* oci)
{
   CPU_DEBUG() << "DEC Handler";

   uint8_t val = emulatorRead(theOperandAddr);

   val--;

   emulatorWrite(theOperandAddr, val);

   if (val == 0)
      theStatusReg.theZeroFlag = 1;

   if (val & 0x80)
      theStatusReg.theSignFlag = 1;
}

void Cpu6502::handler_sed(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sed";
}

void Cpu6502::handler_sta(OpCodeInfo* oci)
{
   CPU_DEBUG() << "STA Handler";

   emulatorWrite(theOperandAddr, theAccum);
}

void Cpu6502::handler_sre(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for sre";
}

void Cpu6502::handler_shx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for shx";
}

void Cpu6502::handler_shy(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for shy";
}

void Cpu6502::handler_bpl(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bpl";
}

void Cpu6502::handler_bcc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bcc";
}

void Cpu6502::handler_cpx(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for cpx";
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

   if (theAccum == 0)
      theStatusReg.theZeroFlag = 1;

   if (theAccum & 0x80)
      theStatusReg.theSignFlag = 1;

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

   if (theAccum == 0)
      theStatusReg.theZeroFlag = 1;

   if (theAccum & 0x80)
      theStatusReg.theSignFlag = 1;

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

void Cpu6502::handler_slo(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for slo";
}

void Cpu6502::handler_las(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for las";
}

void Cpu6502::handler_isc(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for isc";
}

void Cpu6502::handler_brk(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for brk";
}

void Cpu6502::handler_cmp(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for cmp";
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

void Cpu6502::handler_bcs(OpCodeInfo* oci)
{
   CPU_DEBUG() << "Empty handler for bcs";
}





