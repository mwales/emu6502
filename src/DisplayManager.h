#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <iostream>
#include <SDL2/SDL.h>

class Display;
class DisplayDevice;
class Cpu6502;
class MemoryController;

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

   Display* theDisplay;

   DisplayDevice* theDisplayDevice;

   Cpu6502* theCpu;

   MemoryController* theMemoryController;

   SDL_Thread* theEmulationThread;

};

#endif // DISPLAYMANAGER_H
