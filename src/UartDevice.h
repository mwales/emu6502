#ifndef UART_H
#define UART_H

#include "MemoryDev.h"
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

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

   virtual uint8_t read8(CpuAddress absAddr);

   virtual bool write8(CpuAddress absAddr, uint8_t val);

   virtual uint16_t read16(CpuAddress absAddr);

   virtual bool write16(CpuAddress absAddr, uint16_t val);

   // Configuration methods

   virtual bool isFullyConfigured();

   virtual std::vector<std::string> getIntConfigParams();

   virtual std::vector<std::string> getStringConfigParams();

   virtual void setIntConfigValue(std::string paramName, int value);
   virtual void setStringConfigValue(std::string paramName, std::string value);

   virtual void resetMemory();

protected:

   int theConfigFlags;

   int thePortNumber;

   TCPsocket theServerSocket;

   TCPsocket theClientSocket;

   // Lets us do non-blocking socket receiving
   SDLNet_SocketSet theSocketSet;
};

#endif // UART_H
