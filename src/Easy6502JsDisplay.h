#ifndef EASY6502JSDISPLAY_H
#define EASY6502JSDISPLAY_H

#include "DisplayDevice.h"
#include <stdint.h>
#include <map>
#include "DisplayCommands.h"

/**
 * Memory locations $200 to $5ff map to the screen pixels. Different values will
 * draw different colour pixels. The colours are:
 *
 * $0: Black
 * $1: White
 * $2: Red
 * $3: Cyan
 * $4: Purple
 * $5: Green
 * $6: Blue
 * $7: Yellow
 * $8: Orange
 * $9: Brown
 * $a: Light red
 * $b: Dark grey
 * $c: Grey
 * $d: Light green
 * $e: Light blue
 * $f: Light grey
 *
 * The screen is 32 pixels wide
 * The screen is 32 pixels tall
 *
 * $200 0,0 is top left
 * $21f 31,0 is top right
 * $5e0 0,31 is bottom left
 * $5ff 31,31 is bottom right
 */
class Easy6502JsDisplay : public DisplayDevice
{
public:
   Easy6502JsDisplay();

   virtual ~Easy6502JsDisplay();

   virtual uint8_t read8(CpuAddress absAddr);

   virtual bool write8(CpuAddress absAddr, uint8_t val);

   virtual uint16_t read16(CpuAddress absAddr);

   virtual bool write16(CpuAddress absAddr, uint16_t val);

   virtual void resetMemory();

   virtual void startDisplay();

protected:

   std::map<uint8_t, Color24> theColorPalette;

   uint8_t* theDisplayFrame;

   static const int SCREEN_WIDTH = 32;
   static const int SCREEN_HEIGHT = 32;
};

#endif // EASY6502JSDISPLAY_H
