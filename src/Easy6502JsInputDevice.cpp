#include "Easy6502JsInputDevice.h"
#include "EmulatorConfig.h"

#include <cstring>

#include "SimpleQueue.h"
#include "Logger.h"
#include "EmulatorConfig.h"
#include "DisplayCommands.h"

#ifdef EASY6502_INPUTDEV_TRACE
   #define EASYINPUT_DEBUG LOG_DEBUG
   #define EASYINPUT_WARNING   LOG_WARNING
#else
   #define EASYINPUT_DEBUG     if(0) LOG_DEBUG
   #define EASYINPUT_WARNING   if(0) LOG_WARNING
#endif

Easy6502JsInputDevice::Easy6502JsInputDevice():
   MemoryDev("Easy6502 JS Input Device"),
   isKeyPressed(false),
   theCurrentKeyPressed(0)
{
   EASYINPUT_DEBUG() << "Created an instance of the Easy6502JsInputDevice";
   theAddress = 0xff;
   theSize = 1;
}

Easy6502JsInputDevice::~Easy6502JsInputDevice()
{
   EASYINPUT_DEBUG() << "Destroyed an instance of the Easy6502JsInputDevice";
}

uint8_t Easy6502JsInputDevice::read8(CpuAddress offset)
{
   // Flush all the evnets out of the queue
   processEventQueue();

   EASYINPUT_DEBUG() << "read8 called, returning:" << Utils::toHex8(theCurrentKeyPressed);
   return theCurrentKeyPressed;
}

bool Easy6502JsInputDevice::write8(CpuAddress offset, uint8_t val)
{
   EASYINPUT_WARNING() << "Input device is read-only";
   return true;
}

uint16_t Easy6502JsInputDevice::read16(CpuAddress offset)
{
   // This is just a single address, 16-bit operations are invalid
   EASYINPUT_WARNING() << "Easy6502JsInputDevice can't do 16-bit operations (read16)";
   return false;
}

bool Easy6502JsInputDevice::write16(CpuAddress offset, uint16_t val)
{
   // This is just a single address, 16-bit operations are invalid
   EASYINPUT_WARNING() << "Easy6502JsInputDevice can't do 16-bit operations (write16)";
   return false;
}

void Easy6502JsInputDevice::resetMemory()
{

}

void Easy6502JsInputDevice::setCommandQueue(SimpleQueue* cmdQ)
{
   EASYINPUT_DEBUG() << "Easy6502JsInputDevice::setCommandQueue";
   theCommandQueue = cmdQ;
}

void Easy6502JsInputDevice::setEventQueue(SimpleQueue* eventQ)
{
   EASYINPUT_DEBUG() << "Easy6502JsInputDevice::setEventQueue";
   theEventQueue = eventQ;
}

void Easy6502JsInputDevice::setupEventQueue()
{
   DisplayCommand dc;
   dc.id = SUBSCRIBE_SDL_EVENT_TYPE;

   if (theCommandQueue == nullptr)
   {
      EASYINPUT_WARNING() << "Failed to setup event queue, no reference to command queue";
      return;
   }

   EASYINPUT_DEBUG() << "Subscribing to keyup events";
   dc.data.DcSubscribeSdlEventType.eventType = SDL_KEYUP;
   theCommandQueue->writeMessage(sizeof(DisplayCommand), (char*) &dc);

   EASYINPUT_DEBUG() << "Subscribing to keydown events";
   dc.data.DcSubscribeSdlEventType.eventType = SDL_KEYDOWN;
   theCommandQueue->writeMessage(sizeof(DisplayCommand), (char*) &dc);
}

std::string Easy6502JsInputDevice::getConfigTypeName() const
{
   return "Easy6502InputDevice";
}

void Easy6502JsInputDevice::processEventQueue()
{
   EASYINPUT_DEBUG() << "Easy6502JsInputDevice::processEventQueue";

   SDL_Event ev;
   memset(&ev, 0, sizeof(SDL_Event));
   bool keepProcessing = true;
   int numBytesRead = 0;
   while(keepProcessing)
   {
      theEventQueue->tryReadMessage(&numBytesRead, (char*) &ev, sizeof(SDL_Event));

      if (numBytesRead)
      {
         EASYINPUT_DEBUG() << "Found an event in the event queue, size:"
                           << numBytesRead;

         processSdlEvent(ev);
      }
      else
      {
         EASYINPUT_DEBUG() << "No more events in the queue";
         break;
      }
   }
}

void Easy6502JsInputDevice::processSdlEvent(SDL_Event const & ev)
{
   if (ev.type == SDL_KEYUP)
   {
      if ((theCurrentKeyPressed == 'w') && (ev.key.keysym.scancode == SDL_SCANCODE_W))
      {
         theCurrentKeyPressed = 0;
         EASYINPUT_DEBUG() << "Nothing pressed (w released)";
      }
      if ((theCurrentKeyPressed == 'a') && (ev.key.keysym.scancode == SDL_SCANCODE_A))
      {
         theCurrentKeyPressed = 0;
         EASYINPUT_DEBUG() << "Nothing pressed (a released)";
      }
      if ((theCurrentKeyPressed == 's') && (ev.key.keysym.scancode == SDL_SCANCODE_S))
      {
         theCurrentKeyPressed = 0;
         EASYINPUT_DEBUG() << "Nothing pressed (s released)";
      }
      if ((theCurrentKeyPressed == 'd') && (ev.key.keysym.scancode == SDL_SCANCODE_D))
      {
         theCurrentKeyPressed = 0;
         EASYINPUT_DEBUG() << "Nothing pressed (d released)";
      }
   }
   else if (ev.type == SDL_KEYDOWN)
   {
      if (ev.key.keysym.scancode == SDL_SCANCODE_W)
      {
         theCurrentKeyPressed = 'w';
         EASYINPUT_DEBUG() << "w pressed";
      }
      if (ev.key.keysym.scancode == SDL_SCANCODE_A)
      {
         theCurrentKeyPressed = 'a';
         EASYINPUT_DEBUG() << "a pressed";
      }
      if (ev.key.keysym.scancode == SDL_SCANCODE_S)
      {
         theCurrentKeyPressed = 's';
         EASYINPUT_DEBUG() << "s pressed";
      }
      if (ev.key.keysym.scancode == SDL_SCANCODE_D)
      {
         theCurrentKeyPressed = 'd';
         EASYINPUT_DEBUG() << "d pressed";
      }
   }
   else
   {
      EASYINPUT_WARNING() << "Unexpected SDL event type received";
   }
}
