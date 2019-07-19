#ifndef DISPLAY_H
#define DISPLAY_H

#include "MemoryDev.h"
#include "SimpleQueue.h"
#include "SDL2/SDL.h"

#include "DisplayCommands.h"

/**
 * Displays graphics for the user.  Receives command messages from the display
 * device running in the emulation thread.  This class has to be in the SDL
 * GUI thread
 */
class Display
{
public:
   Display();

   virtual ~Display();

   /**
    * Starts the display, blocks until the display is closed
    * @return True if display closed by external cause (not user clicking X button)
    */
   virtual bool startDisplay();

   void setCommandQueue(SimpleQueue* theCmdQ);

protected:

   bool handleDisplayQueueCommand(DisplayCommand* cmd);

   bool handleDcSetResolution(DisplayCommand* cmd);

   bool handleDcClearScreen(DisplayCommand* cmd);

   bool handleDcDrawPixel(DisplayCommand* cmd);

   bool handleDcHaltEmulation(DisplayCommand* cmd);

   SimpleQueue* theDisplayCommandQueue;

   SDL_Window* theWindow;

   uint16_t theLargestCommandSize;

   /** This flag needs to be set if display externally triggered to close (like the emulation is
    * halting on it's own due to an error)
    */
   bool theDisplayClosingExternallyTriggered;

};

#endif // DISPLAY_H
