#ifndef UART_H
#define UART_H

#include "MemoryDev.h"
#include "EmulatorCommon.h"
#include <cstdint>
#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_net.h>

/**
 * A uart device
 *
 * Offset 0x1 is for writing.  Put a character there to write it to the network socket.
 * Offset 0x4 is for reading.  If 6502 reads non-zero, then it was a character from the socket
 */
class UartDevice : public MemoryDev
{
public:
   // Construction methods

   UartDevice(std::string name);

   static MemoryDeviceConstructor getMDC();
   static std::string getTypeName();

   virtual ~UartDevice();

   // Access methods

   virtual uint8_t read8(CpuAddress absAddr) override;

   virtual bool write8(CpuAddress absAddr, uint8_t val) override;

   virtual uint16_t read16(CpuAddress absAddr) override;

   virtual bool write16(CpuAddress absAddr, uint16_t val) override;

   // Configuration methods

   virtual bool isFullyConfigured() const override;

   virtual std::string getConfigTypeName() const;

   virtual void resetMemory() override;

protected:

   int theConfigFlags;

   uint16_t thePortNumber;

   TCPsocket theServerSocket;

   TCPsocket theClientSocket;

   // Lets us do non-blocking socket receiving
   SDLNet_SocketSet theSocketSet;
};

#endif // UART_H
