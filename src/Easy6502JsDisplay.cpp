#include "Easy6502JsDisplay.h"
#include "DisplayCommands.h"
#include "Logger.h"

#ifdef EASY6502_DISPLAY_TRACE
   #define EASY6502_DEBUG LOG_DEBUG
   #define EASY6502_WARNING   LOG_WARNING
#else
   #define EASY6502_DEBUG     if(0) LOG_DEBUG
   #define EASY6502_WARNING   if(0) LOG_WARNING
#endif

Easy6502JsDisplay::Easy6502JsDisplay()
{
   EASY6502_DEBUG() << "Easy6502JsDisplay constructor";
}

Easy6502JsDisplay::~Easy6502JsDisplay()
{
   EASY6502_DEBUG() << "Easy6502JsDisplay destructor";
}

uint8_t Easy6502JsDisplay::read8(CpuAddress offset)
{
   return 0;
}

bool Easy6502JsDisplay::write8(CpuAddress offset, uint8_t val)
{

   return false;
}

uint16_t Easy6502JsDisplay::read16(CpuAddress offset)
{
   return 0;
}

bool Easy6502JsDisplay::write16(CpuAddress offset, uint16_t val)
{

   return false;
}

void Easy6502JsDisplay::resetMemory()
{

}

void Easy6502JsDisplay::startDisplay()
{
   EASY6502_DEBUG() << "Easy6502JsDisplay::startDisplay()";

   // We need to send resolution command to the GUI 640x640 is pixels that are 20x upscaling
   DisplayCommand cmd;
   cmd.id = DisplayCommandId::SET_RESOLUTION;
   cmd.data.DcSetResolution.width = 640;
   cmd.data.DcSetResolution.height = 640;

   theDisplayCommandQueue.writeMessage(sizeof(DisplayCommand), (char*) &cmd);

   cmd.id = DisplayCommandId::SET_LOGICAL_SIZE;
   cmd.data.DcSetLogicalSize.width = 32;
   cmd.data.DcSetLogicalSize.height = 32;
   theDisplayCommandQueue.writeMessage(sizeof(DisplayCommand), (char*) &cmd);

   cmd.id = DisplayCommandId::CLEAR_SCREEN;
   cmd.data.DcClearScreen.blankColor.blue = 0;
   cmd.data.DcClearScreen.blankColor.red = 0;
   cmd.data.DcClearScreen.blankColor.green = 0;
   theDisplayCommandQueue.writeMessage(sizeof(DisplayCommand), (char*) &cmd);
}
