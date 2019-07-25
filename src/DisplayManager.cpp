#include "DisplayManager.h"
#include "EmulatorConfig.h"
#include "Logger.h"

#include "Easy6502JsDisplay.h"
#include "DisplayCommands.h"
#include "SimpleQueue.h"
#include "Display.h"

#include "MemoryController.h"
#include "Cpu6502.h"

#ifdef DISPLAY_MANAGER_TRACE
   #define DM_DEBUG LOG_DEBUG
   #define DM_WARNING   LOG_WARNING
#else
   #define DM_DEBUG     if(0) LOG_DEBUG
   #define DM_WARNING   if(0) LOG_WARNING
#endif

DisplayManager* DisplayManager::theInstancePtr = nullptr;

DisplayManager* DisplayManager::getInstance()
{
   if (theInstancePtr == nullptr)
   {
      theInstancePtr = new DisplayManager();
      return theInstancePtr;
   }
   else
   {
      return theInstancePtr;
   }
}

void DisplayManager::destroyInstance()
{
   if (theInstancePtr == nullptr)
   {
      DM_WARNING() << "Can't destroy an instance that doesn't exist";
   }
   else
   {
      delete theInstancePtr;
      theInstancePtr = nullptr;
   }
}

void emulatorShutdownCallback()
{
   DM_DEBUG() << "DisplayManager emulatorShutdownCallback called";
   DisplayManager::getInstance()->shutdownEmulator();
}


DisplayManager::DisplayManager():
   theDisplay(nullptr),
   theDisplayDevice(nullptr),
   theCpu(nullptr)
{
   DM_DEBUG() << "Creating the singleton DisplayManager";
}

DisplayManager::~DisplayManager()
{
   DM_DEBUG() << "DisplayManager destructor called";

   if (theDisplayDevice != nullptr)
   {
      DM_DEBUG() << "Deleting the display object";
      delete theDisplayDevice;
   }
}

void DisplayManager::setMemoryController(MemoryController* memCtrl)
{
   theMemoryController = memCtrl;
}

void DisplayManager::configureDisplay(std::string displayType, Cpu6502* cpu)
{
   theCpu = cpu;

   DM_DEBUG() << "Configuring display manager with display type:" << displayType;

   if (displayType == "none")
   {
      // Do nothing, there won't be any display
      return;
   }

   if (displayType == "easy6502JsClone")
   {
      // Create the Easy 6502 JS Emulator display
      theDisplayDevice = (DisplayDevice*) new Easy6502JsDisplay();

      // theMemoryController->addNewDevice(theDisplay);
   }

   theDisplay = new Display();
   theDisplay->setCommandQueue(theDisplayDevice->getCommandQueue());

}

bool DisplayManager::isDisplayConfigured()
{
   return (theDisplay != nullptr);
}

void DisplayManager::startEmulator()
{
   DM_DEBUG() << "DisplayManager starting the emulator";

   // Verify the emulator object has been set
   if (theCpu == nullptr)
   {
      DM_WARNING() << "Can't start emulator, display manager doesn't have emulator instance";
      return;
   }

   // Add a way for the emulator to signal to the display that we want to quit
   theCpu->addHaltCallback(emulatorShutdownCallback);

   // Start the emulator and get it running
   theEmulationThread = SDL_CreateThread(DisplayManager::emulationThread, "Emulation", 0 );

   // Open the display / This will until display is closed
   DM_DEBUG() << "DisplayManager starting the display";

   theDisplayDevice->startDisplay();

   bool externallyClosed = theDisplay->startDisplay();

   // The display has closed at this point, did the user close it, or did emulation close it
   if (externallyClosed)
   {
      DM_DEBUG() << "Display closed due to external event, waiting for emulation thread";
      // We don't need to tell the emulator to halt, it already has

   }
   else
   {
      DM_DEBUG() << "Display closed, shutting down emulation";
      theCpu->exitEmulation();
   }

   DM_DEBUG() << "Display manager waiting for emulation thread to exit";
   int threadExitCode;
   SDL_WaitThread(theEmulationThread, &threadExitCode);
}


int DisplayManager::emulationThread(void* data)
{
   // This thread can't access the display at all!
   DisplayManager* dispMgr = DisplayManager::getInstance();

   dispMgr->theCpu->start();

   return 0;
}

void DisplayManager::shutdownEmulator()
{
   if (theDisplayDevice == nullptr)
   {
      DM_WARNING() << "Can't shutdown display, no display device";
      return;
   }

   // Sends shutdown message into Q
   theDisplayDevice->stopDisplay();
}
