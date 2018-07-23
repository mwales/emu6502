#ifndef DEBUG_MESSAGES_H
#define DEBUG_MESSAGES_H

#include <stdint.h>

struct Debugger_Msg
{
   char frameMarker[3]; // C64
   uint8_t msgId;
   uint32_t msgLen;
} DebuggerMsg;

#define DEBUGGER_REG_DUMP    0x1
#define DEBUGGER_REG_WRITE   0x2
#define DEBUGGER_MEM_DUMP    0x3
#define DEBUGGER_MEM_WRITE   0x4
#define DEBUGGER_STEP        0x5
#define DEBUGGER_CONTINUE    0x6
#define DEBUGGER_PAUSE       0x7

struct Debugger_Reg_Dump
{
   uint8_t theRegX;
   uint8_t theRegY;
   uint8_t theAccum;
   uint8_t theStackPtr;
   uint8_t theStatusReg;
   uint16_t thePc;
} DebuggerRegDump;







#endif // DEBUG_MESSAGES_H
