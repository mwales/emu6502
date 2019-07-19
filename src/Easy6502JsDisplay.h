#ifndef EASY6502JSDISPLAY_H
#define EASY6502JSDISPLAY_H

#include "DisplayDevice.h"

class Easy6502JsDisplay : public DisplayDevice
{
public:
   Easy6502JsDisplay();

   virtual ~Easy6502JsDisplay();

   virtual uint8_t read8(CpuAddress offset);

   virtual bool write8(CpuAddress offset, uint8_t val);

   virtual uint16_t read16(CpuAddress offset);

   virtual bool write16(CpuAddress offset, uint16_t val);

   virtual void resetMemory();

   virtual void startDisplay();
};

#endif // EASY6502JSDISPLAY_H
