#include "DisplayDevice.h"

DisplayDevice::DisplayDevice():
   MemoryDev("Display"),
   theDisplayCommandQueue(2048)
{

}


SimpleQueue* DisplayDevice::getCommandQueue()
{
   return &theDisplayCommandQueue;
}
