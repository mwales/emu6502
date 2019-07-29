#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <iostream>
#include <SDL2/SDL.h>

class Display;
class DisplayDevice;
class Cpu6502;
class MemoryController;
class SimpleQueue;

/**
 * Creates the Display object, and the DisplayDevice object (which is specified
 * in the configuration file).  Spawns a separate thread for the emulation to
 * run in, and the emulation talks to the displays via a command and event
 * queue that this class also manages.
 */
class DisplayManager
{
public:
   static DisplayManager* getInstance();

   static void destroyInstance();

   void setMemoryController(MemoryController* memCtrl);

   void configureDisplay(std::string displayType, Cpu6502* cpu);

   bool isDisplayConfigured();

   /**
    * Emulator must be controlled by the dipslay thread.  This method will
    * start the emulation thread and won't return until everything can be
    * closed
    */
   void startEmulator();

   void shutdownEmulator();

protected:

   // Singleton's have constructor and destructor protected
   DisplayManager();
   ~DisplayManager();

   static int emulationThread(void* data);

   static DisplayManager* theInstancePtr;

   /// The actual class that interacts with SDL most for the display
   Display* theDisplay;

   /// The memory mapped display device that interacts with the emulator
   DisplayDevice* theDisplayDevice;

   /// The CPU emulator
   Cpu6502* theCpu;

   /// Memory controller for the emulator
   MemoryController* theMemoryController;

   /// A seperate thread for emulation (GUI/display stuff is on main thread)
   SDL_Thread* theEmulationThread;

   /// A queue that the emulator can use to communicate to the display object
   SimpleQueue* theDisplayCommandQueue;

   /// A queue that sends events from the display back down into the emulator
   SimpleQueue* theEventQueue;

};

#endif // DISPLAYMANAGER_H
