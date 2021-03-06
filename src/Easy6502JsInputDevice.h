#ifndef EASY6502JSINPUTDEVICE_H
#define EASY6502JSINPUTDEVICE_H

#include "MemoryDev.h"
#include <SDL.h>

class SimpleQueue;

/**
 * Class represents a single byte of memory at 0xff.  It subscibes to SDL key up
 * and down events.  It puts the ascii code of wasd at 0xff whenever those keys
 * are pressed, or stores a 0x0 value.
 */
class Easy6502JsInputDevice : public MemoryDev
{
public:
   Easy6502JsInputDevice();

   virtual ~Easy6502JsInputDevice();

   virtual uint8_t read8(CpuAddress offset) override;

   virtual bool write8(CpuAddress offset, uint8_t val) override;

   virtual uint16_t read16(CpuAddress offset) override;

   virtual bool write16(CpuAddress offset, uint16_t val) override;

   virtual void resetMemory() override;

   void setCommandQueue(SimpleQueue* cmdQ);

   void setEventQueue(SimpleQueue* eventQ);

   /// Subscribes to SDL key up and down events
   void setupEventQueue();

   virtual std::string getConfigTypeName() const;

protected:

   void processEventQueue();

   void processSdlEvent(SDL_Event const & ev);

   /// Where we receive our SDL events
   SimpleQueue* theEventQueue;

   /// Command queue where we will tell the display to send us the SDL events we want
   SimpleQueue* theCommandQueue;

   bool isKeyPressed;

   uint8_t theCurrentKeyPressed;
};

#endif // EASY6502JSINPUTDEVICE_H
