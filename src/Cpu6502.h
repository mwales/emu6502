#ifndef CPU6502_H
#define CPU6502_H

#include "EmulatorConfig.h"
#include "Decoder6502.h"
#include <vector>

class DebugServer;
class MemoryController;

typedef void (*HaltFunctionCallback)(void);


#ifdef TRACE_EXECUTION
   #include<stdio.h> // FILE
   class Disassembler6502;
#endif

class Cpu6502 : public Decoder6502
{
public:
    Cpu6502(MemoryController* ctrlr);

    virtual ~Cpu6502();

    virtual void start();

    /**
     * Opens the debug server socket.
     * @param portNumber
     * @return False on errror
     */
    bool enableDebugger(uint16_t portNumber);

#ifdef TRACE_EXECUTION
    /**
     * Set an a limit to the number of steps the emulator will execute before exitting for testing
     * @param numSteps Finite number of steps or 0xffffffffffffffff for infinity
     */
    void setStepLimit(uint64_t numSteps);
#endif

    /**
     * Called from debugger to have the emulator exit
     */
    void exitEmulation();

    void addHaltCallback(HaltFunctionCallback cb);

    // All of the op code handler functions (auto-generated code below)
    virtual void handler_and(OpCodeInfo* oci);
    virtual void handler_bvs(OpCodeInfo* oci);
    virtual void handler_sec(OpCodeInfo* oci);
    virtual void handler_rol(OpCodeInfo* oci);
    virtual void handler_pla(OpCodeInfo* oci);
    virtual void handler_anc(OpCodeInfo* oci);
    virtual void handler_rti(OpCodeInfo* oci);
    virtual void handler_arr(OpCodeInfo* oci);
    virtual void handler_rra(OpCodeInfo* oci);
    virtual void handler_bvc(OpCodeInfo* oci);
    virtual void handler_sax(OpCodeInfo* oci);
    virtual void handler_lsr(OpCodeInfo* oci);
    virtual void handler_rts(OpCodeInfo* oci);
    virtual void handler_inx(OpCodeInfo* oci);
    virtual void handler_ror(OpCodeInfo* oci);
    virtual void handler_ldx(OpCodeInfo* oci);
    virtual void handler_alr(OpCodeInfo* oci);
    virtual void handler_ahx(OpCodeInfo* oci);
    virtual void handler_sei(OpCodeInfo* oci);
    virtual void handler_iny(OpCodeInfo* oci);
    virtual void handler_inc(OpCodeInfo* oci);
    virtual void handler_cli(OpCodeInfo* oci);
    virtual void handler_beq(OpCodeInfo* oci);
    virtual void handler_cpy(OpCodeInfo* oci);
    virtual void handler_cld(OpCodeInfo* oci);
    virtual void handler_txs(OpCodeInfo* oci);
    virtual void handler_tas(OpCodeInfo* oci);
    virtual void handler_clc(OpCodeInfo* oci);
    virtual void handler_adc(OpCodeInfo* oci);
    virtual void handler_tsx(OpCodeInfo* oci);
    virtual void handler_xaa(OpCodeInfo* oci);
    virtual void handler_clv(OpCodeInfo* oci);
    virtual void handler_asl(OpCodeInfo* oci);
    virtual void handler_jmp(OpCodeInfo* oci);
    virtual void handler_bne(OpCodeInfo* oci);
    virtual void handler_ldy(OpCodeInfo* oci);
    virtual void handler_axs(OpCodeInfo* oci);
    virtual void handler_plp(OpCodeInfo* oci);
    virtual void handler_tax(OpCodeInfo* oci);
    virtual void handler_pha(OpCodeInfo* oci);
    virtual void handler_bmi(OpCodeInfo* oci);
    virtual void handler_rla(OpCodeInfo* oci);
    virtual void handler_tya(OpCodeInfo* oci);
    virtual void handler_tay(OpCodeInfo* oci);
    virtual void handler_sbc(OpCodeInfo* oci);
    virtual void handler_lax(OpCodeInfo* oci);
    virtual void handler_txa(OpCodeInfo* oci);
    virtual void handler_jsr(OpCodeInfo* oci);
    virtual void handler_kil(OpCodeInfo* oci);
    virtual void handler_bit(OpCodeInfo* oci);
    virtual void handler_php(OpCodeInfo* oci);
    virtual void handler_nop(OpCodeInfo* oci);
    virtual void handler_dcp(OpCodeInfo* oci);
    virtual void handler_ora(OpCodeInfo* oci);
    virtual void handler_dex(OpCodeInfo* oci);
    virtual void handler_dey(OpCodeInfo* oci);
    virtual void handler_dec(OpCodeInfo* oci);
    virtual void handler_sed(OpCodeInfo* oci);
    virtual void handler_sta(OpCodeInfo* oci);
    virtual void handler_sre(OpCodeInfo* oci);
    virtual void handler_shx(OpCodeInfo* oci);
    virtual void handler_shy(OpCodeInfo* oci);
    virtual void handler_bpl(OpCodeInfo* oci);
    virtual void handler_bcc(OpCodeInfo* oci);
    virtual void handler_cpx(OpCodeInfo* oci);
    virtual void handler_eor(OpCodeInfo* oci);
    virtual void handler_lda(OpCodeInfo* oci);
    virtual void handler_slo(OpCodeInfo* oci);
    virtual void handler_las(OpCodeInfo* oci);
    virtual void handler_isc(OpCodeInfo* oci);
    virtual void handler_brk(OpCodeInfo* oci);
    virtual void handler_cmp(OpCodeInfo* oci);
    virtual void handler_stx(OpCodeInfo* oci);
    virtual void handler_sty(OpCodeInfo* oci);
    virtual void handler_bcs(OpCodeInfo* oci);
    // End of auto generated code

    virtual void addition_operation(uint8_t operandValue, OpCodeInfo* oci);

    virtual void comparison_operation(uint8_t regValue, uint8_t operandValue);

    virtual void halt();

    // CPU State Getters

    void       getRegisters(uint8_t* regX, uint8_t* retY, uint8_t* accum);
    CpuAddress getPc();
    uint8_t    getStackPointer();
    uint8_t    getStatusReg();
    uint64_t   getInstructionCount();

    /**
     * Our implementation of decode calls debugger hook and makes sure emulator
     * allowed to run
     */
    virtual int decodeWithDebugging();

    /// Runs fast as possible, only will stop when we are told or when emulator breaks
    virtual int decode();

protected:

    virtual void updatePc(uint8_t bytesIncrement);

    void preHandlerHook(OpCodeInfo* oci);
    int postHandlerHook(OpCodeInfo* oci);

    /**
     * Safely tries to read memory.  If their is no valid memory device for the
     * address the emulator tried to execute, then execution is halted
     * @param addr
     * @return Data at the address, or 0xff during a failure.
     */
    uint8_t emulatorRead(CpuAddress addr);

    /**
     * Safely tries to write a byte of memory.  If their is no valid memory
     * device for the address, the emulator is halted.
     */
    void emulatorWrite(CpuAddress addr, uint8_t val);

    /// The preHandlerHook will set theOperandVal and theOperandAddr based on the
    /// addressing mode
    uint8_t    theOperandVal;
    CpuAddress theOperandAddr;

    // Core emulator registers
    uint8_t theRegX;
    uint8_t theRegY;
    uint8_t theAccum;
    uint8_t theStackPtr;
    StatusReg theStatusReg;

    /// The debugger if one is configured, else it is a nullptr
    DebugServer* theDebugger;

    /// Separate flag for the debugger to make the emulator shut down
    bool theDebuggerShutdownFlag;

    bool theRunFlag;

    std::vector<HaltFunctionCallback> theHaltCallbacksList;

    /**
     * The number of clock cycles the CPU has been on for
     */
    uint64_t theNumClocks;

    /**
     * Extra clock cycles induced per instruction based on addressing modes
     */
    int theAddrModeExtraClockCycle;

    /**
     * If the memory address computed during an operation crossed over a page
     * boundary, it might cause an extra clock cycle for the instruction to
     * complete
     */
    bool thePageBoundaryCrossedFlag;

#ifdef TRACE_EXECUTION
    /// Used to create disassembler listings for each statement
    Disassembler6502* theDisAss;

    FILE* theTraceFile;

    uint64_t theNumberOfStepsToTrace;
    uint64_t theNumberOfStepsExecuted;
#endif

};

#endif // CPU6502_H
