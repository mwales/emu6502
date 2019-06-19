#include "DebuggerState.h"
#include "EmulatorConfig.h"
#include "Logger.h"

#ifdef DSTATE_DEBUG_LOGGING
   #define DSTATE_DEBUG   LOG_DEBUG
   #define DSTATE_WARNING LOG_WARNING
#else
   #define DSTATE_DEBUG   if(0) LOG_DEBUG
   #define DSTATE_WARNING if(0) LOG_WARNING
#endif

DebuggerState::DebuggerState():
   theStepCount(0),
   theHaltFired(false),
   theHaltCallbackValue(0)
{
   theLock = SDL_CreateSemaphore(1);
}

bool DebuggerState::emulatorDebugHook()
{
   DSTATE_DEBUG() << "DebuggerState::emualtorDebugHook";

   SDL_SemWait(theLock);

   if (theStepCount > 0)
   {
      theStepCount--;\
      SDL_SemPost(theLock);

      DSTATE_DEBUG() << "  emulatorDebugHook has " << theStepCount << " finite steps left";
      return true;
   }

   if (theStepCount == -1)
   {
      SDL_SemPost(theLock);

      DSTATE_DEBUG() << "  emulatorDebugHook is in run forever mode";
      return true;
   }

   // Step Count == 0, Did we just halt?
   if (!theHaltFired)
   {
      SDL_SemPost(theLock);

      theHaltFired = true;

      DSTATE_DEBUG() << "  emulatorDebugHook detected a fresh halt";

      // @todo WE need to fire the halt
      theHaltCallback(theHaltCallbackValue);

      return false;
   }
   else
   {
      // We already halted before
      SDL_SemPost(theLock);

      DSTATE_DEBUG() << "  emulatorDebugHook halted, but not a fresh halt";

      return false;
   }


}

void DebuggerState::haltEmulator()
{
   SDL_SemWait(theLock);

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

