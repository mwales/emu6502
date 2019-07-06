#include "Decoder6502.h"
#include "EmulatorConfig.h"
#include "Logger.h"
#include "Utils.h"
#include "MemoryDev.h"
#include "MemoryController.h"
#include "Cpu6502Defines.h"

#ifdef DECODER_6502_DEBUG
   #define DECODER_DEBUG     LOG_DEBUG
   #define DECODER_WARNING   LOG_WARNING
#else
   #define DECODER_DEBUG     if(0) LOG_DEBUG
   #define DECODER_WARNING   if(0) LOG_WARNING
#endif

int Decoder6502::decode()
{
   DECODER_DEBUG() << "Decoder6502::decode(" << Utils::toHex16(thePc) << ")";

   MemoryDev* mem = theMemoryController->getDevice(thePc);

   if (mem == 0)
   {
      halt();
      DECODER_WARNING() << "Decoding failed, no memory device for address" << addressToString(thePc);

      // Dump out all the memory devices
      for(auto& singleDev : theMemoryController->getAllDevices())
      {
         DECODER_DEBUG() << " MemoryDev: " << singleDev->getDebugString();
      }

      return -1;
   }

   uint8_t opCode = mem->read8(thePc);

   OpCodeInfo* oci = &gOpCodes[opCode];

   if (oci->theNumBytes >= 2)
      theOpCode2 = theMemoryController->getDevice(thePc + 1)->read8(thePc + 1);

   if (oci->theNumBytes >= 3)
      theOpCode3 = theMemoryController->getDevice(thePc + 2)->read8(thePc + 2);

   // Call the handler for the function!
   DECODER_DEBUG() << "Found opCode " << Utils::toHex8(opCode)
                   << " @ addr " << Utils::toHex16(thePc);

   preHandlerHook(oci);

   auto handlerFunc = oci->theOpCodeHandler;
   (*this.*handlerFunc)(oci);

   int clockCycles = postHandlerHook(oci);

   updatePc(oci->theNumBytes);

   return clockCycles;
}

void Decoder6502::preHandlerHook(OpCodeInfo* oci)
{
   // This is where sub-classes will do sub-class specific behavior if necessary
   DECODER_DEBUG() << "Decoder6502::preHandlerHook called";
}

int Decoder6502::postHandlerHook(OpCodeInfo* oci)
{
   // This is where sub-classes will do sub-class specific behavior if necessary
   DECODER_DEBUG() << "Decoder6502::postHandlerHook called";

   // Default for non-emulator decoders is 0 clock cycles
   return 0;
}
