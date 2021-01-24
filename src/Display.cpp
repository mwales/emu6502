#include "Display.h"
#include "Debugger.h"

#include <cstring>
#include <cstdlib>

#include "Logger.h"
#include "Utils.h"


#ifdef DISPLAYWINDOW_DEBUG
   #define DISP_DEBUG     LOG_DEBUG
   #define DISP_WARNING   LOG_WARNING
#else
   #define DISP_DEBUG     if(0) LOG_DEBUG
   #define DISP_WARNING   if(0) LOG_WARNING
#endif

#ifdef SDL_TRACE_DEBUG
   #define SDL_TRACE    LOG_DEBUG
#else
   #define SDL_TRACE    if(0) LOG_DEBUG
#endif

Display* Display::theInstancePtr = nullptr;

Display* Display::createInstance()
{
   if (theInstancePtr != nullptr)
   {
      DISP_WARNING() << "createInstance called when instance pointer already created";
   }

   theInstancePtr = new Display();
   return theInstancePtr;
}

Display* Display::getInstance()
{
   if (theInstancePtr == nullptr)
   {
      DISP_WARNING() << "getInstance called with theInstancePtr null";
   }

   return theInstancePtr;
}

void Display::destroyInstance()
{
   if (theInstancePtr == nullptr)
   {
      DISP_WARNING() << "destroyInstance called with theInstancePtr null";
   }

   delete theInstancePtr;
   theInstancePtr = nullptr;
}


Display::Display():
   theDisplayCommandQueue(nullptr),
   theEventCommandQueue(nullptr),
   theWindow(nullptr),
   theRenderer(nullptr),
   theDisplayClosingExternallyTriggered(false)
{
   DISP_DEBUG() << "Display constructor called";

   theDisplayCommandQueue = new SimpleQueue(1024);
   theEventCommandQueue = new SimpleQueue(1024);
}

Display::~Display()
{
   DISP_DEBUG() << "destructor called";

   if (theRenderer != nullptr)
   {
      DISP_DEBUG() << "Destroy SDL renderer";
      SDL_DestroyRenderer(theRenderer);
      theRenderer = nullptr;
   }

   if(theWindow != nullptr)
   {
      DISP_DEBUG() << "Destroy SDL window";
      SDL_DestroyWindow(theWindow);
      theWindow = nullptr;
   }
}

bool Display::startDisplay()
{
   DISP_DEBUG() << "Display::startDisplay()";

   if (theDisplayCommandQueue == nullptr)
   {
      DISP_WARNING() << "startDisplay called with no command queue set!";
      return false;
   }

   DisplayCommand cmd;
   memset((char*) &cmd, 0, sizeof(DisplayCommand));

   // Have a flag for loop since can't next break statements
   bool runFlag = true;
   int numBytesInCommand = 0;
   bool callSuccessful = true;
   while(runFlag)
   {
      DISP_DEBUG() << "Starting loop of display";

      // Clear all the events in the display command queue
      do
      {
         callSuccessful = theDisplayCommandQueue->tryReadMessage(&numBytesInCommand,
                                                                 (char*) &cmd,
                                                                 sizeof(DisplayCommand));

         DISP_DEBUG() << "DISP RX: " << Utils::hexDump((uint8_t*) &cmd, numBytesInCommand);

         if (!callSuccessful)
         {
            DISP_WARNING() << "Error trying to read display command queue";
            break;
         }

         if (numBytesInCommand == 0)
         {
            // There are no commands left, so nothing
            DISP_DEBUG() << "No Display commands ready at this point";
         }
         else if (numBytesInCommand != sizeof(DisplayCommand))
         {
            DISP_WARNING() << "Received command in Display Q with wrong size.  Size = "
                           << numBytesInCommand << "not" << sizeof(DisplayCommand);
         }
         else
         {
            // Message received is the correct size for a DisplayCommand
            runFlag = handleDisplayQueueCommand(&cmd);
         }

      } while (numBytesInCommand > 0);

      SDL_TRACE() << "SDL_RenderPresent(pointer)";
      SDL_RenderPresent(theRenderer);

      // Check for events from SDL
      int sdlCallSuccess;
      SDL_Event ev;
      sdlCallSuccess = SDL_WaitEventTimeout(&ev, 10);
      SDL_TRACE() << "SDL_WaitEventTimeout(pointer,1000)";

      if (sdlCallSuccess)
      {
         switch(ev.type)
         {
         case SDL_KEYDOWN:
            DISP_DEBUG() << "Received Keydown event";
            break;

         case SDL_KEYUP:
            DISP_DEBUG() << "Received Keyup event";
            break;

         case SDL_QUIT:
            DISP_DEBUG() << "Received Quit event";
            return false;  // user closed by clicking X
            break;

         default:
            DISP_DEBUG() << "Received unexpected SDL event type:" << sdlEventTypeToString(ev.type);
         }

         if (isSdlEventInteresting(ev.type))
         {
            DISP_DEBUG() << "Found an interesting event";
            theEventCommandQueue->writeMessage(sizeof(SDL_Event), (char*) &ev);
         }


      }
      else
      {
         DISP_DEBUG() << "No SDL event received";
      }

   } // end while

   return true;  // we are closing because emulator told us too
}

bool Display::processQueues()
{
   DISP_DEBUG() << "Display processing queues";

   DisplayCommand cmd;
   memset((char*) &cmd, 0, sizeof(DisplayCommand));

   bool runFlag = true;
   int numBytesInCommand = 0;
   bool callSuccessful = true;

   // Clear all the events in the display command queue
   do
   {
      callSuccessful = theDisplayCommandQueue->tryReadMessage(&numBytesInCommand,
                                                              (char*) &cmd,
                                                              sizeof(DisplayCommand));

      if (!callSuccessful)
      {
         DISP_WARNING() << "Error trying to read display command queue";
         break;
      }

      DISP_DEBUG() << "DISP RX: " << Utils::hexDump((uint8_t*) &cmd, numBytesInCommand);

      if (numBytesInCommand == 0)
      {
         // There are no commands left, so nothing
         DISP_DEBUG() << "No Display commands ready at this point";
      }
      else if (numBytesInCommand != sizeof(DisplayCommand))
      {
         DISP_WARNING() << "Received command in Display Q with wrong size.  Size = "
                        << numBytesInCommand << "not" << sizeof(DisplayCommand);
      }
      else
      {
         // Message received is the correct size for a DisplayCommand
         runFlag = handleDisplayQueueCommand(&cmd);
      }

   } while (numBytesInCommand > 0);

   SDL_TRACE() << "SDL_RenderPresent(pointer)";
   SDL_RenderPresent(theRenderer);

   // Check for events from SDL
   int sdlCallSuccess;
   SDL_Event ev;
   sdlCallSuccess = SDL_WaitEventTimeout(&ev, 10);
   SDL_TRACE() << "SDL_WaitEventTimeout(pointer,1000)";

   if (sdlCallSuccess)
   {
      switch(ev.type)
      {
      case SDL_KEYDOWN:
         DISP_DEBUG() << "Received Keydown event";
         break;

      case SDL_KEYUP:
         DISP_DEBUG() << "Received Keyup event";
         break;

      case SDL_QUIT:
         DISP_DEBUG() << "Received Quit event";
         runFlag = false;  // user closed by clicking X
         break;

      default:
         DISP_DEBUG() << "Received unexpected SDL event type:" << sdlEventTypeToString(ev.type);
      }

      if (isSdlEventInteresting(ev.type))
      {
         DISP_DEBUG() << "Found an interesting event";
         theEventCommandQueue->writeMessage(sizeof(SDL_Event), (char*) &ev);
      }


   }
   else
   {
      DISP_DEBUG() << "No SDL event received";
   }

   return runFlag;
}

bool Display::handleDisplayQueueCommand(DisplayCommand* cmd)
{
   DISP_DEBUG() << "Handling display queue event" << (int) cmd->id;

   switch(cmd->id)
   {
   case HALT_EMULATION:
      return handleDcHaltEmulation(cmd);

   case SET_RESOLUTION:
      return handleDcSetResolution(cmd);

   case SET_LOGICAL_SIZE:
      return handleDcSetLogicalSize(cmd);

   case CLEAR_SCREEN:
      return handleDcClearScreen(cmd);

   case DRAW_PIXEL:
      return handleDcDrawPixel(cmd);

   case SUBSCRIBE_SDL_EVENT_TYPE:
      return handleDcSdlSubscribeEventType(cmd);

   case NO_DISPLAY_DEVICE:
      return handleNoDisplayDevice(cmd);

   default:
      DISP_WARNING() << "Invalid command ID sent to display" << (int) cmd->id;
      return false;
   }
}

void Display::setCommandQueue(SimpleQueue* cmdQ)
{
   DISP_WARNING() << "Display has received a reference to the command queue (OLD API USE)";
   theDisplayCommandQueue = cmdQ;
}

void Display::setEventQueue(SimpleQueue* eventQ)
{
   DISP_WARNING() << "Display has received a reference to the event queue (OLD API USE)";
   theEventCommandQueue = eventQ;
}


SimpleQueue* Display::getCommandQueue()
{
   return theDisplayCommandQueue;
}

SimpleQueue* Display::getEventQueue()
{
   return theEventCommandQueue;
}

bool Display::handleDcSetResolution(DisplayCommand* cmd)
{
   return setResolution(cmd->data.DcSetResolution.width, cmd->data.DcSetResolution.height);
}

bool Display::setResolution(int width, int height)
{
   DISP_DEBUG() << "Display has received a set resolution command";

   if(theWindow != nullptr)
   {
      DISP_WARNING() << "There was an SDL window already created and we received set resolution command";
   }
   else
   {
      // Time to create the SDL window
      DISP_DEBUG() << "Creating SDL window";

      SDL_TRACE() << "SDL_CreateWindow(\"EMU 6502\", SDL_WINDOWPOS_CENTERED, SDLWINDOWPOS_CENTERED,"
                  << width << "," << height << ",0)";

      theWindow = SDL_CreateWindow("EMU 6502", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   width, height, 0);

      if (theWindow == NULL)
      {
         LOG_FATAL() << "Error creating the SDL window: " << SDL_GetError();
      }

      SDL_TRACE() << "SDL_CreateRenderer(pointer, -1, SDL_RENDERER_SOFTWARE)";
      theRenderer = SDL_CreateRenderer(theWindow, -1, SDL_RENDERER_SOFTWARE);

      if (theRenderer == NULL)
      {
         LOG_FATAL() << "Error creating the SDL renderer:" << SDL_GetError();
         return true;
      }
   }

   return true;
}

bool Display::handleDcSetLogicalSize(DisplayCommand* cmd)
{
   return setLogicalSize(cmd->data.DcSetLogicalSize.width,
                         cmd->data.DcSetLogicalSize.height);
}

bool Display::setLogicalSize(int width, int height)
{
   if ( (theWindow == nullptr) || (theRenderer == nullptr) )
   {
      LOG_FATAL() << "Can't set logical size of renderer if SDL window not initialized";
      return true;
   }

   SDL_TRACE() << "SDL_RenderSetLogicalSize(pointer," << width << "," << height << ")";

   if (SDL_RenderSetLogicalSize(theRenderer, width, height))
   {
      DISP_WARNING() << "Error setting logical renderer size:" << SDL_GetError();
   }
   else
   {
      DISP_DEBUG() << "Set SDL Renderer to " << width << "x" << height << "successfully";
   }

   return true;
}

bool Display::handleDcClearScreen(DisplayCommand* cmd)
{
   return clearScreen(cmd->data.DcClearScreen.blankColor);
}

bool Display::clearScreen(Color24 bgcolor)
{
   DISP_DEBUG() << "Display has received a clear screen command";

   SDL_TRACE() << "SDL_SetRenderDrawColor(pointer," << Utils::toHex8(bgcolor.red)
               << "," << Utils::toHex8(bgcolor.green) << ","
               << Utils::toHex8(bgcolor.blue) << ",SDL_ALPHA_OPAQUE)";

   if (SDL_SetRenderDrawColor(theRenderer, bgcolor.red, bgcolor.green,
                              bgcolor.blue, SDL_ALPHA_OPAQUE))
   {
      DISP_WARNING() << "Error setting the color of renderer before clearing screen:"
                     << SDL_GetError();
   }

   SDL_TRACE() << "SDL_RenderClear(pointer)";

   if (SDL_RenderClear(theRenderer))
   {
      DISP_WARNING() << "Error clearing the screen:" << SDL_GetError();
   }

   return true;
}



bool Display::handleDcDrawPixel(DisplayCommand* cmd)
{
   return drawPixel(cmd->data.DcDrawPixel.x, cmd->data.DcDrawPixel.y,
                    cmd->data.DcDrawPixel.color);
}

bool Display::drawPixel(int x, int y, Color24 col)
{
   DISP_DEBUG() << "Display has received a draw pixel command";

   SDL_TRACE() << "SDL_SetRenderDrawColor(pointer," << Utils::toHex8(col.red)
               << "," << Utils::toHex8(col.green)
               << "," << Utils::toHex8(col.blue) << ",SDL_ALPHA_OPAQUE)";

   if (SDL_SetRenderDrawColor(theRenderer,
                              col.red,
                              col.green,
                              col.blue,
                              SDL_ALPHA_OPAQUE))
   {
       DISP_WARNING() << "drawPixel failed to set color";
       return true;
   }

   SDL_TRACE() << "SDL_RenderDrawPoint(pointer," << x << "," << y << ")";

   if (SDL_RenderDrawPoint(theRenderer, x, y))
   {
       DISP_WARNING() << "drawPixel failed to draw pixel after setting color";
       return true;
   }

   SDL_RenderPresent(theRenderer);

   DISP_DEBUG() << "  pixel @ " << x << "," << y << ", color = " << colorToString(col);
   return true;
}

bool Display::handleDcHaltEmulation(DisplayCommand* cmd)
{
   haltEmulation();
   return false;
}

bool Display::haltEmulation()
{
   DISP_DEBUG() << "Display has received a halt emulation command";
   return false;
}

bool Display::handleDcSdlSubscribeEventType(DisplayCommand* cmd)
{
    DISP_DEBUG() << "Display has received a command to subscribe to SDL event:"
                 << sdlEventTypeToString(cmd->data.DcSubscribeSdlEventType.eventType);
    theEventsOfInterest.push_back(cmd->data.DcSubscribeSdlEventType.eventType);

    return true;
}

bool Display::handleNoDisplayDevice(DisplayCommand* cmd)
{
   if(theWindow == nullptr)
   {
      DISP_WARNING() << "There was an SDL window already created and we received No Display Device command";
      return true;
   }

   // Time to create the SDL window
   DISP_DEBUG() << "Creating SDL window";

   SDL_TRACE() << "SDL_CreateWindow(\"EMU 6502\", SDL_WINDOWPOS_CENTERED, SDLWINDOWPOS_CENTERED,"
               << cmd->data.DcSetResolution.width << "," << cmd->data.DcSetResolution.height << ",0)";

   theWindow = SDL_CreateWindow("EMU 6502", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                640, 480, 0);

   if (theWindow == NULL)
   {
      LOG_FATAL() << "Error creating the SDL window: " << SDL_GetError();
   }

   SDL_TRACE() << "SDL_CreateRenderer(pointer, -1, SDL_RENDERER_SOFTWARE)";
   theRenderer = SDL_CreateRenderer(theWindow, -1, SDL_RENDERER_SOFTWARE);

   if (theRenderer == NULL)
   {
      LOG_FATAL() << "Error creating the SDL renderer:" << SDL_GetError();
      return true;
   }

   if (SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE))
   {
      DISP_WARNING() << "Error setting the color of renderer before clearing screen:"
                     << SDL_GetError();
   }

   SDL_TRACE() << "SDL_RenderClear(pointer)";

   if (SDL_RenderClear(theRenderer))
   {
      DISP_WARNING() << "Error clearing the screen:" << SDL_GetError();
   }

   return true;
}

std::string Display::sdlEventTypeToString(const uint32_t& et)
{
   switch(et)
   {
   case SDL_QUIT:
      return "SDL_QUIT";
   case SDL_APP_TERMINATING:
      return "SDL_APP_TERMINATING";
   case SDL_APP_LOWMEMORY:
      return "SDL_APP_LOWMEMORY";
   case SDL_APP_WILLENTERBACKGROUND:
      return "SDL_APP_WILLENTERBACKGROUND";
   case SDL_APP_DIDENTERBACKGROUND:
      return "SDL_APP_DIDENTERBACKGROUND";
   case SDL_APP_WILLENTERFOREGROUND:
      return "SDL_APP_WILLENTERFOREGROUND";
   case SDL_APP_DIDENTERFOREGROUND:
      return "SDL_APP_DIDENTERFOREGROUND";
   case SDL_WINDOWEVENT:
      return "SDL_WINDOWEVENT";
   case SDL_SYSWMEVENT:
      return "SDL_SYSWMEVENT";
   case SDL_KEYDOWN:
      return "SDL_KEYDOWN";
   case SDL_KEYUP:
      return "SDL_KEYUP";
   case SDL_TEXTEDITING:
      return "SDL_TEXTEDITING";
   case SDL_TEXTINPUT:
      return "SDL_TEXTINPUT";
   case SDL_KEYMAPCHANGED:
      return "SDL_KEYMAPCHANGED";
   case SDL_MOUSEMOTION:
      return "SDL_MOUSEMOTION";
   case SDL_MOUSEBUTTONDOWN:
      return "SDL_MOUSEBUTTONDOWN";
   case SDL_MOUSEBUTTONUP:
      return "SDL_MOUSEBUTTONUP";
   case SDL_MOUSEWHEEL:
      return "SDL_MOUSEWHEEL";
   case SDL_JOYAXISMOTION:
      return "SDL_JOYAXISMOTION";
   case SDL_JOYBALLMOTION:
      return "SDL_JOYBALLMOTION";
   case SDL_JOYHATMOTION:
      return "SDL_JOYHATMOTION";
   case SDL_JOYBUTTONDOWN:
      return "SDL_JOYBUTTONDOWN";
   case SDL_JOYBUTTONUP:
      return "SDL_JOYBUTTONUP";
   case SDL_JOYDEVICEADDED:
      return "SDL_JOYDEVICEADDED";
   case SDL_JOYDEVICEREMOVED:
      return "SDL_JOYDEVICEREMOVED";
   case SDL_CONTROLLERAXISMOTION:
      return "SDL_CONTROLLERAXISMOTION";
   case SDL_CONTROLLERBUTTONDOWN:
      return "SDL_CONTROLLERBUTTONDOWN";
   case SDL_CONTROLLERBUTTONUP:
      return "SDL_CONTROLLERBUTTONUP";
   case SDL_CONTROLLERDEVICEADDED:
      return "SDL_CONTROLLERDEVICEADDED";
   case SDL_CONTROLLERDEVICEREMOVED:
      return "SDL_CONTROLLERDEVICEREMOVED";
   case SDL_CONTROLLERDEVICEREMAPPED:
      return "SDL_CONTROLLERDEVICEREMAPPED";
   case SDL_FINGERDOWN:
      return "SDL_FINGERDOWN";
   case SDL_FINGERUP:
      return "SDL_FINGERUP";
   case SDL_FINGERMOTION:
      return "SDL_FINGERMOTION";
   case SDL_DOLLARGESTURE:
      return "SDL_DOLLARGESTURE";
   case SDL_DOLLARRECORD:
      return "SDL_DOLLARRECORD";
   case SDL_MULTIGESTURE:
      return "SDL_MULTIGESTURE";
   case SDL_CLIPBOARDUPDATE:
      return "SDL_CLIPBOARDUPDATE";
   case SDL_DROPFILE:
      return "SDL_DROPFILE";
   case SDL_DROPTEXT:
      return "SDL_DROPTEXT";
   case SDL_DROPBEGIN:
      return "SDL_DROPBEGIN";
   case SDL_DROPCOMPLETE:
      return "SDL_DROPCOMPLETE";
   case SDL_AUDIODEVICEADDED:
      return "SDL_AUDIODEVICEADDED";
   case SDL_AUDIODEVICEREMOVED:
      return "SDL_AUDIODEVICEREMOVED";
   case SDL_RENDER_TARGETS_RESET:
      return "SDL_RENDER_TARGETS_RESET";
   case SDL_RENDER_DEVICE_RESET:
      return "SDL_RENDER_DEVICE_RESET";
   case SDL_USEREVENT:
      return "SDL_USEREVENT";
   default:
      return "*** UNKNOWN SDL EVENT ***";
   }
}

bool Display::isSdlEventInteresting(uint32_t const & et)
{
   for(auto & etIt: theEventsOfInterest)
   {
      if (et == etIt)
          return true;
   }

   return false;
}

std::string Display::colorToString(Color24 c)
{
    std::string retVal = "";
    retVal += "(";
    retVal += Utils::toHex8(c.red, false);
    retVal += ",";
    retVal += Utils::toHex8(c.green, false);
    retVal += ",";
    retVal += Utils::toHex8(c.blue, false);
    retVal += ")";

    return retVal;
}

void Display::registerDebuggerCommands(Debugger* d)
{
   d->registerNewCommandHandler("displayres", "Sets the resolution of display screen",
                                Display::setResolutionDebugHandlerStatic, this);

   d->registerNewCommandHandler("drawpixel", "Draws a pixel on screen",
                                Display::drawPixelDebugHandlerStatic, this);
}

void Display::setResolutionDebugHandlerStatic(std::vector<std::string> const & args, void* context)
{
   Display* disp = reinterpret_cast<Display*>(context);
   disp->setResolutionDebugHandler(args);
}

void Display::setResolutionDebugHandler(std::vector<std::string> const & args)
{
   DISP_DEBUG() << "Display Set Resolution Command called!";
}

void Display::drawPixelDebugHandlerStatic(std::vector<std::string> const & args, void* context)
{
   Display* disp = reinterpret_cast<Display*>(context);
   disp->drawPixelDebugHandler(args);
}

void Display::drawPixelDebugHandler(std::vector<std::string> const & args)
{
   // args are x, y, r, g , b
   if (args.size() != 5)
   {
      std::cout << "Usage: drawPixel x y redVal greenVal blueVal" << std::endl;
      return;
   }

   int x, y;
   uint8_t r, g, b;
   x = atoi(args[0].c_str());
   y = atoi(args[1].c_str());
   r = (uint8_t) atoi(args[2].c_str());
   g = (uint8_t) atoi(args[3].c_str());
   b = (uint8_t) atoi(args[4].c_str());

   Color24 col = {r, g, b};
   drawPixel(x, y, col);
}
