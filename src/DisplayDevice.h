#ifndef DISPLAYDEVICE_H
#define DISPLAYDEVICE_H

#include "MemoryDev.h"
#include "SimpleQueue.h"

/**
 * Common generic parent type for display memory devices
 */
class DisplayDevice : public MemoryDev
{
public:
   DisplayDevice(std::string const & name);

   virtual void startDisplay() = 0;

   virtual void stopDisplay();

   void setCommandQueue(SimpleQueue* cmdQ);

   void setEventQueue(SimpleQueue* eventQ);

protected:

   SimpleQueue* theDisplayCommandQueue;

   SimpleQueue* theEventQueue;

};

#endif // DISPLAYDEVICE_H
