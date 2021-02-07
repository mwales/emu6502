#ifndef DISPLAY_H
#define DISPLAY_H

#include "MemoryDev.h"
#include "SimpleQueue.h"
#include "SDL.h"

#include "DisplayCommands.h"
#include "DisplayEvents.h"

#include <vector>
#include <string>

class Debugger;

/**
 * Displays graphics for the user.  Receives command messages from the display
 * device running in the emulation thread.  This class has to be in the SDL
 * GUI thread
 */
class Display
{
public:

   static Display* createInstance();

   static Display* getInstance();

   static void destroyInstance();

   /**
    * Starts the display, blocks until the display is closed
    * @return True if display closed by external cause (not user clicking X button)
    */
   virtual bool startDisplay();

   void setCommandQueue(SimpleQueue* cmdQ);

   void setEventQueue(SimpleQueue* eventQ);

   SimpleQueue* getCommandQueue();

   SimpleQueue* getEventQueue();

   void registerDebuggerCommands(Debugger* d);

   bool processQueues();

   static void setResolutionDebugHandlerStatic(std::vector<std::string> const & args, void* context);

   void setResolutionDebugHandler(std::vector<std::string> const & args);

   static void drawPixelDebugHandlerStatic(std::vector<std::string> const & args, void* context);

   void drawPixelDebugHandler(std::vector<std::string> const & args);

   void render();

   // Command Functions
   bool setResolution(int width, int height);
   bool setLogicalSize(int width, int height);
   bool clearScreen(Color24 bgcolor);
   bool drawPixel(int x, int y, Color24 col);
   bool haltEmulation();

protected:


   Display();

   virtual ~Display();

   bool handleDisplayQueueCommand(DisplayCommand* cmd);

   bool handleDcSetResolution(DisplayCommand* cmd);

   bool handleDcSetLogicalSize(DisplayCommand* cmd);

   bool handleDcClearScreen(DisplayCommand* cmd);

   bool handleDcDrawPixel(DisplayCommand* cmd);

   bool handleDcHaltEmulation(DisplayCommand* cmd);

   bool handleDcSdlSubscribeEventType(DisplayCommand* cmd);

   bool handleNoDisplayDevice(DisplayCommand* cmd);

   void sendKeyStateChangeEvent(bool isDown, SDL_Keysym keycode);

   std::string sdlEventTypeToString(const uint32_t& et);

   bool isSdlEventInteresting(const uint32_t & et);

   std::string colorToString(Color24 c);

   /// Commands sent from the emulator to the display telling the display what to show
   SimpleQueue* theDisplayCommandQueue;

   /// Evnets from SDL are put in this queue for the emulator
   SimpleQueue* theEventCommandQueue;

   SDL_Window* theWindow;

   SDL_Renderer* theRenderer;

   /** This flag needs to be set if display externally triggered to close (like the emulation is
    * halting on it's own due to an error)
    */
   bool theDisplayClosingExternallyTriggered;

   std::vector<uint32_t> theEventsOfInterest;

   static Display* theInstancePtr;

};

#endif // DISPLAY_H
