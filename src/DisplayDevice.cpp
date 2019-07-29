#include "DisplayDevice.h"
#include "DisplayCommands.h"

DisplayDevice::DisplayDevice(std::string const & name):
   MemoryDev(name),
   theDisplayCommandQueue(nullptr),
   theEventQueue(nullptr)
{

}

void DisplayDevice::stopDisplay()
{
   DisplayCommand haltCmd;
   haltCmd.id = DisplayCommandId::HALT_EMULATION;
   theDisplayCommandQueue->writeMessage(sizeof(DisplayCommand), (char*) &haltCmd);
}


void DisplayDevice::setCommandQueue(SimpleQueue* cmdQ)
{
   theDisplayCommandQueue = cmdQ;
}

void DisplayDevice::setEventQueue(SimpleQueue* evQ)
{
   theEventQueue = evQ;
}
