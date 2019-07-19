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
   theState(PAUSE),
   theStepsLeft(-1)
{
   DSTATE_DEBUG() << "DebuggerState object initialized";
}

DebuggerState::~DebuggerState()
{
   DSTATE_DEBUG() << "DebuggerState destructor called";
}

bool DebuggerState::emulatorAllowExecution()
{
   DSTATE_DEBUG() << "DebuggerState::emulatorDebugHook";

   switch(theState)
   {
   case PAUSE:
   case FRESH_HALT:
      // We are paused, don't emulate
      DSTATE_DEBUG() << "DebuggerState::emulatorDebugHook - PAUSED";
      return false;

   case RUN:
      // We are running, emulate away
      DSTATE_DEBUG() << "DebuggerState::emulatorDebugHook - RUN";
      return true;

   case STEPPING:
      if (theStepsLeft == 0)
      {
         DSTATE_DEBUG() << "DebuggerState::emulatorDebugHook - STEPPING (Breakpoing Hit!)";
         theState = FRESH_HALT;
         return false;
      }
      else
      {
         theStepsLeft--;
         DSTATE_DEBUG() << "DebuggerState::emulatorDebugHook - STEPPING (" << theStepsLeft
                        << "instructions left)";
         return true;
      }
   }

   // Impossible to get to
   return true;
}

void DebuggerState::pauseEmulator()
{
   DSTATE_DEBUG() << "pauseEmulator called";
   theStepsLeft = 0;
   theState = FRESH_HALT;
}

void DebuggerState::stepEmulator(int steps)
{
   if (steps <= 0)
   {
      LOG_FATAL() << "DebuggerState::stepEmulator() called with negative invalid steps: " << steps;
      steps = 1;
   }

   DSTATE_DEBUG() << "stepEmulator(" << steps << ") called";
   theStepsLeft = steps;
   theState = STEPPING;
}

void DebuggerState::runEmulator()
{
   DSTATE_DEBUG() << "runEmulator called";
   theState = RUN;
   theStepsLeft = -1;
}

void DebuggerState::acknowledgeHalt()
{
   DSTATE_DEBUG() << "acknowledgeHalt called";
   theState = PAUSE;
   theStepsLeft = -1;
}

bool DebuggerState::isFreshHalt()
{
   DSTATE_DEBUG() << "isFreshHalt called:" << (theState == FRESH_HALT ? "TRUE" : "FALSE");
   return (theState == FRESH_HALT);
}
