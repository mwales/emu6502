#ifndef DEBUGGER_STATE_H
#define DEBUGGER_STATE_H

#include <SDL2/SDL.h>

class DebuggerState
{
public:

   DebuggerState();

   /**
    * This method is called by the main debug hook and will block the emulator
    * if the debugger wants the emulator blocked
    * @returns False if the debugger should sleep for a bit and block
    */
   bool emulatorDebugHook();

   /**
    * The debugger thread calls this to cause the emulator to block the next
    * time the debugger hook is called
    */
   void haltEmulator();

   /**
    * Lets the emulator run for a finite number of steps before halting itself
    */
   void stepEmulator(int steps);

   /**
    * Tells the emulator to run without blocking on the debugger (until the
    * user explicitly halts emulator, or emulation hits an error
    */
   void runEmulator();

   /**
    * Allows the debugger to register a callback that it wants to be called
    * when the emulator is halted to due to the halt command, or number of
    * steps have been executed
    */
   void registerEmulatorHaltedCallback(void (*callback)(void*), void* callbackValue);

protected:

   SDL_sem* theLock;

   int theStepCount;

   bool theHaltFired;

   void (*theHaltCallback)(void*);

   void* theHaltCallbackValue;
};

#endif // DEBUGGER_STATE_H

