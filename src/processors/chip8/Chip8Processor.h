#ifndef CHIP8_PROCESSOR_H
#define CHIP8_PROCESSOR_H

#include "Chip8Decoder.h"
#include "Chip8Display.h"
#include "Processor.h"

#include <vector>
#include <set>
#include <map>

#include "SimpleQueue.h"

class Chip8Disassembler;
class Display;

/**
 * Chip-8 Processor class
 */
class Chip8Processor : public Chip8Decoder, public Processor
{
public:
   Chip8Processor(std::string instanceName);
   virtual ~Chip8Processor();
   
   // Accessors for the Chip-8 interpreter / CPU internal state
   unsigned char getRegister(unsigned char reg);
   unsigned int getIP();
   unsigned int getIndexRegister();
   unsigned char getDelayTimer();
   
   std::vector<unsigned int> getStack();
   
   // Controls to play, pause, reset the emulator
   void resetState();
   bool step();
   
   // void enableSound(bool enable);
   
   virtual std::vector<std::string> getRegisterNames();
   
   virtual uint32_t getRegisterValue(std::string regName);
   
   virtual bool setRegisterValue(std::string regName, uint32_t value);
   
   virtual uint32_t getInstructionLength(CpuAddress addr);
   
   virtual int disassembleAddr(CpuAddress addr, std::string* listingDataRet);
   
   virtual std::string getCpuName();
   
   // From the decoder
   virtual bool getByteFromAddress(CpuAddress address, uint8_t* retByte);

   // ISerializable
   virtual uint32_t getSaveStateLength() override;
   virtual bool saveState(uint8_t* buffer, uint32_t* bytesSaved) override;
   virtual bool loadState(uint8_t* buffer, uint32_t* bytesLoaded) override;

   virtual void registerDebugHandlerCommands(Debugger* dbgr);

   void breakpointCommandHandler(std::vector<std::string> const & args);
   void breakpointListCommandHandler(std::vector<std::string> const & args);
   void breakpointDeleteCommandHandler(std::vector<std::string> const & args);
   void loadChip8RomCommandHandler(std::vector<std::string> const & args);

protected:
   
   // Internal state variables for the interpreter
   std::vector<unsigned char> theCpuRegisters;
   std::vector<unsigned int> theCpuStack;

   unsigned int theIndexRegister;

   // QDateTime theDelayTimerExpiration;
   uint32_t theTimerStartTickVal;
   uint8_t theTimerValOriginal;

   std::vector<unsigned char> theHp48Flags;
   
   // Key press event lock and list
   void checkDisplayEvents();
   std::set<uint8_t> theKeysDown;
   
   Chip8Disassembler* theDisassembler;

   Chip8Display theDisplay;

   CpuAddress theLowResFontsAddr;
   CpuAddress theHiResFontsAddr;
   
   bool theSoundEnabled;
   
   bool theStopFlag;

   /**
    * Breakpoints are going to be implemented as CALL #000.  So when inserting a breakpoint
    * replace the instruction with a CALL 0, but put the real instruction in this table so it
    * can be executed later.
    */
   std::map<CpuAddress, uint16_t> theBreakpointList;

   /**
     * Whenever you stop for a breakpoint, put the instruction counter here.  Don't stop for
     * a breakpoint you just stopped for on previous instruction
     */
   uint64_t theCounterOnBreakpoint;
   
   /**
    * Loads the Chip-8 font sprites into memory below 0x200
    */
   void loadFonts();

   bool load5BytesHelper(CpuAddress& addr, uint8_t byteA, uint8_t byteB,
                         uint8_t byteC, uint8_t byteD, uint8_t byteE);

   // Chip-8 instruction handlers
   
   void insClearScreen();
   
   void insReturnFromSub();
   
   void insJump(unsigned addr);
   
   void insCall(unsigned addr);
   
   void insSetIndexReg(unsigned addr);
   
   void insJumpWithOffset(unsigned addr);
   
   void insSkipNextIfRegEqConst(unsigned reg, unsigned val);
   
   void insSkipNextIfRegNotEqConst(unsigned reg, unsigned val);
   
   void insSetReg(unsigned reg, unsigned val);
   
   void insAddReg(unsigned reg, unsigned val);
   
   void insRandomNum(unsigned reg, unsigned mask);
   
   void insSkipNextIfRegEq(unsigned reg1, unsigned reg2);
   
   void insSkipNextIfRegNotEq(unsigned reg1, unsigned reg2);
   
   void insSetRegToRegVal(unsigned regToSet, unsigned regVal);
   
   void insOrReg(unsigned reg, unsigned otherReg);
   
   void insAndReg(unsigned reg, unsigned otherReg);
   
   void insXorReg(unsigned reg, unsigned otherReg);
   
   void insAddRegs(unsigned reg, unsigned otherReg);
   
   void insSubRegs(unsigned reg, unsigned otherReg);
   
   void insSubRegsOtherOrder(unsigned reg, unsigned otherReg);
   
   void insRightShift(unsigned reg);
   
   void insLeftShift(unsigned reg);
   
   void insSkipNextIfKeyPressed(unsigned reg);
   
   void insSkipNextIfKeyNotPressed(unsigned reg);
   
   void insWaitForKeyPress(unsigned reg);
   
   void insSetRegToDelayTimer(unsigned reg);
   
   void insSetDelayTimer(unsigned reg);
   
   void insSetSoundTimer(unsigned reg);
   
   void insAddRegToIndexReg(unsigned reg);
   
   void insSetIndexToCharInReg(unsigned reg);
   
   void insSetIndexMemoryToRegBcd(unsigned reg);
   
   void insStoreRegsToIndexMemory(unsigned reg);
   
   void insLoadRegsFromIndexMemory(unsigned reg);
   
   void insStoreRegsToIndexMemoryXo(unsigned regStart, unsigned regStop);
   
   void insLoadRegsFromIndexMemoryXo(unsigned regStart, unsigned regStop);
   
   void insDrawSprite(unsigned xReg, unsigned yReg, unsigned height);
   
   // Super chip-8 instructions
   void insScrollDown(unsigned char numLines);
   
   void insScrollUp(unsigned char numLines);  // XO-Spec Instruction
   
   void insScrollLeft();
   
   void insScrollRight();
   
   void insQuitEmulator();
   
   void insEnterLowResMode();
   
   void insEnterHighResMode();
   
   void insSaveHp48Flags(unsigned char reg);
   
   void insLoadHp48Flags(unsigned char reg);
   
   void insSetIndexToHiResCharInReg(unsigned char reg);
   
   void insGraphicsPlane(unsigned char planeNum);
   
   void insBad(unsigned opCode);

   SimpleQueue* theEventQueue;

};

#endif // CHIP8_PROCESSOR_H
