#include "DebuggerState.h"
#include "Logger.h"

DebuggerState::DebuggerState():
   theStepCount(0),
   theHaltFired(false),
   theHaltCallbackValue(0)
{
   theLock = SDL_CreateSemaphore(1);
}

bool DebuggerState::emulatorDebugHook()
{
   LOG_DEBUG() << "DebuggerState::emualtorDebugHook";

   SDL_SemWait(theLock);

   if (theStepCount > 0)
   {
      theStepCount--;\
      SDL_SemPost(theLock);
      return false;
   }

   if (theStepCount == -1)
   {
      SDL_SemPost(theLock);
      return true;
   }

   // Step Count == 0, Did we just halt?
   if (!theHaltFired)
   {
      SDL_SemPost(theLock);

      theHaltFired = true;

      // @todo WE need to fire the halt
      theHaltCallback(theHaltCallbackValue);

      return false;
   }
   else
   {
      // We already halted before
      SDL_SemPost(theLock);
      return false;
   }


}

void DebuggerState::haltEmulator()
{
   SDL_SemWait(theLock);

   theHaltFired = false;
   theStepCount = 0;

   SDL_SemPost(theLock);
}

void DebuggerState::stepEmulator(int steps)
{
   if (steps <= 0)
   {
      LOG_FATAL() << "DebuggerState::stepEmulator() called with negative invalid steps: " << steps;
      steps = 1;
   }

   SDL_SemWait(theLock);
   theHaltFired = false;
   theStepCount = steps;
   SDL_SemPost(theLock);
}

void DebuggerState::runEmulator()
{
   SDL_SemWait(theLock);
   theHaltFired = false;
   theStepCount = -1;
   SDL_SemPost(theLock);
}

void DebuggerState::registerEmulatorHaltedCallback(void (*callback)(void*), void* callbackValue)
{
   theHaltCallbackValue = callbackValue;
   theHaltCallback = callback;
}

