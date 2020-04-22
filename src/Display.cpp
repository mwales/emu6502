#include "Display.h"

#include <cstring>

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


Display::Display():
   theDisplayCommandQueue(nullptr),
   theEventCommandQueue(nullptr),
   theWindow(nullptr),
   theRenderer(nullptr),
   theDisplayClosingExternallyTriggered(false)
{
   DISP_DEBUG() << "Display constructor called";
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
   DISP_DEBUG() << "Display has received a reference to the command queue";
   theDisplayCommandQueue = cmdQ;
}

void Display::setEventQueue(SimpleQueue* eventQ)
{
    DISP_DEBUG() << "Display has received a reference to the event queue";
    theEventCommandQueue = eventQ;
}

bool Display::handleDcSetResolution(DisplayCommand* cmd)
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
                  << cmd->data.DcSetResolution.width << "," << cmd->data.DcSetResolution.height << ",0)";

      theWindow = SDL_CreateWindow("EMU 6502", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   cmd->data.DcSetResolution.width,
                                   cmd->data.DcSetResolution.height, 0);

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
   if ( (theWindow == nullptr) || (theRenderer == nullptr) )
   {
      LOG_FATAL() << "Can't set logical size of renderer if SDL window not initialized";
      return true;
   }

   SDL_TRACE() << "SDL_RenderSetLogicalSize(pointer," << cmd->data.DcSetLogicalSize.width
               << "," << cmd->data.DcSetLogicalSize.height << ")";

   if (SDL_RenderSetLogicalSize(theRenderer, cmd->data.DcSetLogicalSize.width,
                                cmd->data.DcSetLogicalSize.height))
   {
      DISP_WARNING() << "Error setting logical renderer size:" << SDL_GetError();
   }
   else
   {
      DISP_DEBUG() << "Set SDL Renderer to " << cmd->data.DcSetLogicalSize.width << "x"
                   << cmd->data.DcSetLogicalSize.height << "successfully";
   }

   return true;
}

bool Display::handleDcClearScreen(DisplayCommand* cmd)
{
   DISP_DEBUG() << "Display has received a clear screen command";

   SDL_TRACE() << "SDL_SetRenderDrawColor(pointer," << Utils::toHex8(cmd->data.DcClearScreen.blankColor.red)
               << "," << Utils::toHex8(cmd->data.DcClearScreen.blankColor.green)
               << "," << Utils::toHex8(cmd->data.DcClearScreen.blankColor.blue) << ",SDL_ALPHA_OPAQUE)";

   if (SDL_SetRenderDrawColor(theRenderer,
                              cmd->data.DcClearScreen.blankColor.red,
                              cmd->data.DcClearScreen.blankColor.green,
                              cmd->data.DcClearScreen.blankColor.blue,
                              SDL_ALPHA_OPAQUE))
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
   DISP_DEBUG() << "Display has received a draw pixel command";

   SDL_TRACE() << "SDL_SetRenderDrawColor(pointer," << Utils::toHex8(cmd->data.DcDrawPixel.color.red)
               << "," << Utils::toHex8(cmd->data.DcDrawPixel.color.green)
               << "," << Utils::toHex8(cmd->data.DcDrawPixel.color.blue) << ",SDL_ALPHA_OPAQUE)";

   if (SDL_SetRenderDrawColor(theRenderer,
                              cmd->data.DcDrawPixel.color.red,
                              cmd->data.DcDrawPixel.color.green,
                              cmd->data.DcDrawPixel.color.blue,
                              SDL_ALPHA_OPAQUE))
   {
       DISP_WARNING() << "drawPixel failed to set color";
       return true;
   }

   SDL_TRACE() << "SDL_RenderDrawPoint(pointer," << cmd->data.DcDrawPixel.x
               << "," << cmd->data.DcDrawPixel.y << ")";

   if (SDL_RenderDrawPoint(theRenderer, cmd->data.DcDrawPixel.x,
                            cmd->data.DcDrawPixel.y))
   {
       DISP_WARNING() << "drawPixel failed to draw pixel after setting color";
       return true;
   }

   DISP_DEBUG() << "  pixel @ " << cmd->data.DcDrawPixel.x << "," << cmd->data.DcDrawPixel.y
                << ", color = " << colorToString(cmd->data.DcDrawPixel.color);
   return true;
}

bool Display::handleDcHaltEmulation(DisplayCommand* cmd)
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
