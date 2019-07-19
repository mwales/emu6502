#include "Display.h"
#include "Logger.h"


#ifdef DISPLAYWINDOW_DEBUG
   #define DISP_DEBUG     LOG_DEBUG
   #define DISP_WARNING   LOG_WARNING
#else
   #define DISP_DEBUG     if(0) LOG_DEBUG
   #define DISP_WARNING   if(0) LOG_WARNING
#endif


Display::Display():
   theDisplayCommandQueue(nullptr),
   theWindow(nullptr),
   theDisplayClosingExternallyTriggered(false)
{

   theLargestCommandSize = getSizeOfLargestDisplayCommand();

}

Display::~Display()
{
   DISP_DEBUG() << "destructor called";

}

bool Display::startDisplay()
{
   DISP_DEBUG() << "Display::startDisplay()";

   if (theDisplayCommandQueue == nullptr)
   {
      DISP_WARNING() << "startDisplay called with no command queue set!";
      return false;
   }

   DisplayCommand* cmd = (DisplayCommand*) malloc(theLargestCommandSize);
   memset(cmd, 0, theLargestCommandSize);

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
                                                                 (char*) cmd,
                                                                 theLargestCommandSize);
         if (!callSuccessful)
         {
            DISP_WARNING() << "Error trying to read display command queue";
            break;
         }

         runFlag = handleDisplayQueueCommand(cmd);
      } while (numBytesInCommand > 0);

      // Check for events from SDL
      int sdlCallSuccess;
      SDL_Event ev;
      sdlCallSuccess = SDL_WaitEventTimeout(&ev, 10);
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
            break;

         default:
            DISP_DEBUG() << "Received unexpected SDL event type";
         }
      }
      else
      {
         DISP_DEBUG() << "No SDL event received";
      }

   } // end while loop

   free(cmd);

   return true;
   //return theDisplayClosingExternallyTriggered;
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

   case CLEAR_SCREEN:
      return handleDcClearScreen(cmd);

   case DRAW_PIXEL:
      return handleDcDrawPixel(cmd);

   default:
      DISP_WARNING() << "Invalid command ID sent to display" << (int) cmd->id;
      return false;
   }
}

void Display::setCommandQueue(SimpleQueue* theCmdQ)
{
   DISP_DEBUG() << "Display has received a reference to the command queue";
   theDisplayCommandQueue = theCmdQ;
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
      theWindow = SDL_CreateWindow("EMU 6502", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                   cmd->data.DcSetResolution.width, cmd->data.DcSetResolution.height, 0);
   }

   return true;
}

bool Display::handleDcClearScreen(DisplayCommand* cmd)
{
   DISP_DEBUG() << "Display has received a clear screen command";
   return true;
}

bool Display::handleDcDrawPixel(DisplayCommand* cmd)
{
   DISP_DEBUG() << "Display has received a draw pixel command";
   return true;
}

bool Display::handleDcHaltEmulation(DisplayCommand* cmd)
{
   DISP_DEBUG() << "Display has received a halt emulation command";
   return false;
}
