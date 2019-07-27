#include "Easy6502JsDisplay.h"
#include "DisplayCommands.h"
#include "MemoryController.h"
#include "Logger.h"

#ifdef EASY6502_DISPLAY_TRACE
   #define EASY6502_DEBUG LOG_DEBUG
   #define EASY6502_WARNING   LOG_WARNING
#else
   #define EASY6502_DEBUG     if(0) LOG_DEBUG
   #define EASY6502_WARNING   if(0) LOG_WARNING
#endif

Easy6502JsDisplay::Easy6502JsDisplay():
   DisplayDevice("Easy JS 6502 Display")
{
   EASY6502_DEBUG() << "Easy6502JsDisplay constructor";

   theAddress = 0x200;
   theSize = SCREEN_WIDTH * SCREEN_HEIGHT;

   theColorPalette[0] = { 0x00, 0x00, 0x00};   // Black
   theColorPalette[1] = { 0xff, 0xff, 0xff};   // White
   theColorPalette[2] = { 0x88, 0x00, 0x00};   // Red
   theColorPalette[3] = { 0xaa, 0xff, 0xee};   // Cyan
   theColorPalette[4] = { 0xcc, 0x44, 0xcc};   // Purple
   theColorPalette[5] = { 0x00, 0xcc, 0x55};   // Green
   theColorPalette[6] = { 0x00, 0x00, 0xaa};   // Blue
   theColorPalette[7] = { 0xee, 0xee, 0x77};   // Yellow
   theColorPalette[8] = { 0xdd, 0x88, 0x55};   // Orange
   theColorPalette[9] = { 0x66, 0x44, 0x00};   // Brown
   theColorPalette[10] = { 0xff, 0x77, 0x77};  // Light Red
   theColorPalette[11] = { 0x33, 0x33, 0x33};  // Dark Grey
   theColorPalette[12] = { 0x77, 0x77, 0x77};  // Grey
   theColorPalette[13] = { 0xaa, 0xff, 0x66};  // Light Green
   theColorPalette[14] = { 0x00, 0x88, 0xff};  // Light Blue
   theColorPalette[15] = { 0xbb, 0xbb, 0xbb};  // Light Grey

   theDisplayFrame = (uint8_t*) malloc(theSize);
   memset(theDisplayFrame, 0, theSize);
}

Easy6502JsDisplay::~Easy6502JsDisplay()
{
   EASY6502_DEBUG() << "Easy6502JsDisplay destructor";
}

uint8_t Easy6502JsDisplay::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      EASY6502_WARNING() << "Address" << addressToString(absAddr) << "out of range for Easy6502Js display (read8)";
      return 0;
   }

   return theDisplayFrame[absAddr - theAddress];

}

bool Easy6502JsDisplay::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
       EASY6502_WARNING() << "Address" << addressToString(absAddr) << "out of range for Easy6502Js display (write8)";
       return false;
   }

   int offset = absAddr - theAddress;
   theDisplayFrame[offset] = val;


   DisplayCommand dc;
   dc.id = DRAW_PIXEL;
   dc.data.DcDrawPixel.x = offset % SCREEN_WIDTH;
   dc.data.DcDrawPixel.y = offset / SCREEN_WIDTH;

   Color24 pixelColor = theColorPalette[val & 0xf];

   dc.data.DcDrawPixel.color.red = pixelColor.red;
   dc.data.DcDrawPixel.color.green = pixelColor.green;
   dc.data.DcDrawPixel.color.blue = pixelColor.blue;

   theDisplayCommandQueue.writeMessage(sizeof(DisplayCommand), (char*) &dc);

   return true;
}

uint16_t Easy6502JsDisplay::read16(CpuAddress absAddr)
{
   return 0;
}

bool Easy6502JsDisplay::write16(CpuAddress absAddr, uint16_t val)
{

   return false;
}

void Easy6502JsDisplay::resetMemory()
{

}

void Easy6502JsDisplay::startDisplay()
{
   EASY6502_DEBUG() << "Easy6502JsDisplay::startDisplay()";

   theMemController->addNewDevice(this);

   // We need to send resolution command to the GUI 640x640 is pixels that are 20x upscaling
   DisplayCommand cmd;
   cmd.id = DisplayCommandId::SET_RESOLUTION;
   cmd.data.DcSetResolution.width = SCREEN_WIDTH * 20;
   cmd.data.DcSetResolution.height = SCREEN_HEIGHT * 20;

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
