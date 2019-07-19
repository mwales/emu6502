#ifndef DISPLAYDEVICE_H
#define DISPLAYDEVICE_H

#include "MemoryDev.h"
#include "SimpleQueue.h"

class DisplayDevice : public MemoryDev
{
public:
   DisplayDevice();

   virtual void startDisplay() = 0;

   SimpleQueue* getCommandQueue();

protected:

   SimpleQueue theDisplayCommandQueue;

};

#endif // DISPLAYDEVICE_H
