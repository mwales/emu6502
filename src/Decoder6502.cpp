#include "Decoder6502.h"
#include "Logger.h"
#include "Utils.h"
#include "MemoryDev.h"
#include "MemoryController.h"
#include "Cpu6502Defines.h"


void Decoder6502::decode(CpuAddress address)
{
   LOG_DEBUG() << "Decoder6502::decode(" << Utils::toHex16(address) << ")";

   MemoryDev* mem = theMemoryController->getDevice(address);

   if (mem == 0)
   {
      halt();
      LOG_WARNING() << "Decoding failed, no memory device for address" << addressToString(address);
      return;
   }

   uint8_t opCode = mem->read8(address);

   OpCodeInfo* oci = &gOpCodes[opCode];

   if (oci->theNumBytes >= 2)
      theOpCode2 = theMemoryController->getDevice(address + 1)->read8(address + 1);

   if (oci->theNumBytes >= 3)
      theOpCode3 = theMemoryController->getDevice(address + 2)->read8(address + 2);

   // Call the handler for the function!
   LOG_DEBUG() << "Found opCode " << Utils::toHex8(opCode) << " @ addr " << Utils::toHex16(address);

   preHandlerHook(oci);

   auto handlerFunc = oci->theOpCodeHandler;

   (*this.*handlerFunc)(oci);

   postHandlerHook(oci);

   updatePc(oci->theNumBytes);
}

void Decoder6502::preHandlerHook(OpCodeInfo* oci)
{
   // This is where sub-classes will do sub-class specific behavior if necessary
   LOG_DEBUG() << "Decoder6502::preHandlerHook called";
}

void Decoder6502::postHandlerHook(OpCodeInfo* oci)
{
   // This is where sub-classes will do sub-class specific behavior if necessary
   LOG_DEBUG() << "Decoder6502::postHandlerHook called";
}
