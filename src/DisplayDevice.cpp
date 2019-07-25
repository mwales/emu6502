#include "DisplayDevice.h"
#include "DisplayCommands.h"

DisplayDevice::DisplayDevice():
   MemoryDev("Display"),
   theDisplayCommandQueue(2048)
{

}

void DisplayDevice::stopDisplay()
{
   DisplayCommand haltCmd;
   haltCmd.id = DisplayCommandId::HALT_EMULATION;
   theDisplayCommandQueue.writeMessage(sizeof(DisplayCommand), (char*) &haltCmd);
}


SimpleQueue* DisplayDevice::getCommandQueue()
{
   return &theDisplayCommandQueue;
}
