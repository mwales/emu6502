#ifndef DEBUGGER_STATE_H
#define DEBUGGER_STATE_H


/**
 * Keeps track of what state the debugger should be in (and how many steps until the next
 * breakpoint when stepping
 */
class DebuggerState
{
public:

   DebuggerState();

   ~DebuggerState();

   /**
    * This method is called by the main debug hook and will block the emulator
    * if the debugger wants the emulator blocked
    * @returns False if the debugger should sleep for a bit and block
    */
   bool emulatorAllowExecution();

   /**
    * The debugger calls this to cause the state machine to block the next time
    * the debugger hook is called
    */
   void pauseEmulator();

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
    * When the emulation is halted while debugging, the debugger needs to send
    * a dump of stuff to the debugger client.  This is called after the dump
    * of state has been sent to the client
    */
   void acknowledgeHalt();

   bool isFreshHalt();

   bool isPaused();

protected:

   typedef enum
   {
      PAUSE,
      RUN,
      STEPPING,
      FRESH_HALT
   } DebuggerInternalState;

   DebuggerInternalState theState;

   int theStepsLeft;

};

#endif // DEBUGGER_STATE_H

