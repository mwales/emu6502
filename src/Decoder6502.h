#ifndef DECODER6502_H
#define DECODER6502_H

#include "Decoder.h"

/**
 * MOS 6502 instruction decoding.  This class is abstract because it will not implement the halt()
 * operation
 */
class Decoder6502 : public Decoder
{
public:

    virtual void start(CpuAddress address) = 0;

protected:

    virtual void decode(CpuAddress address);

    virtual void load(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void store(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void transfer(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void add(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void subtract(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void increment(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void decrement(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void andOperation(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void orOperation(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void xorOperation(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void shiftLeft(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void shiftRight(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void rotateLeft(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void rotateRight(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void push(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void pull(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void jump(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void jumpSubroutine(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void returnFromInterrupt(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void returnFromSubroutine(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void bitTest(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void compare(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void clear(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void set(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void noOp(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void breakOperation(CpuAddress instAddr, uint8_t opCodes) = 0;

    virtual void branch(CpuAddress instAddr, uint8_t opCodes) = 0;




};

#endif // DECODER6502_H
