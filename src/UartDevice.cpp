#include "UartDevice.h"
#include "EmulatorConfig.h"
#include "Utils.h"
#include "Logger.h"

#ifdef UART_TRACE
   #define UART_DEBUG   LOG_DEBUG
   #define UART_WARNING LOG_WARNING
#else
   #define UART_DEBUG   if(0) LOG_DEBUG
   #define UART_WARNING if(0) LOG_WARNING
#endif

MemoryDev* uartDeviceConstructor(std::string name)
{
   return new UartDevice(name);
}

UartDevice::UartDevice(std::string name):
   MemoryDev(name),
   theConfigFlags(0),
   thePortNumber(0),
   theServerSocket(NULL),
   theClientSocket(NULL)
{
   UART_DEBUG() << "UART Device named" << name << "created";

   theSocketSet = SDLNet_AllocSocketSet(1);

   theUint16ConfigParams.emplace("portNumber", &thePortNumber);
   theUint16ConfigParams.emplace("startAddress", &theAddress);
}

MemoryDeviceConstructor UartDevice::getMDC()
{
   return uartDeviceConstructor;
}

std::string UartDevice::getTypeName()
{
   return "UART";
}

UartDevice::~UartDevice()
{
   if (theClientSocket)
   {
      UART_DEBUG() << "Closing the client socket";
      SDLNet_TCP_Close(theClientSocket);
      theClientSocket = NULL;
   }
   else
   {
      UART_WARNING() << "There was no client socket to close";
   }

   if (theServerSocket)
   {
      UART_DEBUG() << "Closing the server socket";
      SDLNet_TCP_Close(theServerSocket);
      theServerSocket = NULL;
   }
   else
   {
      UART_DEBUG() << "No server socket to close";
   }

   SDLNet_FreeSocketSet(theSocketSet);
}

// Access methods

uint8_t UartDevice::read8(CpuAddress absAddr)
{
   if ( (absAddr - theAddress) != 4)
   {
      UART_WARNING() << "Invalid UART read at offset" << (absAddr - theAddress);
      return 0;
   }

   if (theClientSocket)
   {
      int numReady = SDLNet_CheckSockets(theSocketSet, 0);
      if (numReady == 0)
      {
         UART_DEBUG() << "UART Read called, but no data ready";

      }
      else
      {
         uint8_t val;
         int bytesRead = SDLNet_TCP_Recv(theClientSocket, &val, 1);
         if (bytesRead != 1)
         {
            // Failure during the receive
            UART_WARNING() << "UART Read called, data was ready, but had an error: "
                           << SDLNet_GetError();
         }
         else
         {
            UART_DEBUG() << "UART Read: " << (char) val << " = " << Utils::toHex8(val);
            return val;
         }
      }
   }

   return 0;
}

bool UartDevice::write8(CpuAddress absAddr, uint8_t val)
{
   if ( (absAddr - theAddress) != 1)
   {
      UART_WARNING() << "Invalid UART write at offset" << (absAddr - theAddress);
      return false;
   }
   else
   {
      UART_DEBUG() << "UART Write: " << (char) val << " = " << Utils::toHex8(val);

      if (theClientSocket)
      {
         int bytesSent = SDLNet_TCP_Send(theClientSocket, &val, 1);

         if (bytesSent != 1)
         {
            UART_WARNING() << "There was a problem sending data to the UART client: "
                           << SDLNet_GetError();
         }
      }

      return true;
   }
}

uint16_t UartDevice::read16(CpuAddress absAddr)
{
   UART_WARNING() << "UART doesn't support 16-bit reads";
   return 0;
}

bool UartDevice::write16(CpuAddress absAddr, uint16_t val)
{
   UART_WARNING() << "UART doesn't support 16-bit writes";
   return false;
}

// Configuration methods

// RAM configuration flags
#define UART_ADDR_CONFIG 0x01
#define UART_PORT_CONFIG 0x02
#define UART_CONFIG_DONE (UART_ADDR_CONFIG | UART_PORT_CONFIG)

bool UartDevice::isFullyConfigured() const
{
   return (theConfigFlags == UART_CONFIG_DONE);
}

std::string UartDevice::getConfigTypeName() const
{
   return getTypeName();
}

std::vector<std::string> UartDevice::getIntConfigParams() const
{
   std::vector<std::string> retVal;
   retVal.push_back("portNumber");
   retVal.push_back("startAddress");
   return retVal;
}

std::vector<std::string> UartDevice::getStringConfigParams() const
{
   std::vector<std::string> retVal;
   return retVal;
}

void UartDevice::setIntConfigValue(std::string paramName, int value)
{
   if (paramName == "portNumber")
   {
      UART_DEBUG() << "UART" << theName << "configured for TCP port" << value;
      theConfigFlags |= UART_PORT_CONFIG;
      thePortNumber = value;
   }

   if (paramName == "startAddress")
   {
      UART_DEBUG() << "UART" << theName << "configured at address" << addressToString(value);
      theConfigFlags |= UART_ADDR_CONFIG;
      theAddress = value;
   }
}

void UartDevice::setStringConfigValue(std::string paramName, std::string value)
{
   // Empty
}

void UartDevice::resetMemory()
{
   if(!isFullyConfigured())
   {
      UART_WARNING() << "reset aborting due to insufficient configuration";
      return;
   }

   // At this point we should be fully configured so we can setup socket
   theSize = 0x10;

   // Create socket
   IPaddress ip;
   if (SDLNet_ResolveHost(&ip, NULL, thePortNumber) != 0)
   {
      UART_WARNING() << "ResolveHost call failed:" << SDLNet_GetError();
      return;
   }

   theServerSocket = SDLNet_TCP_Open(&ip);
   if (!theServerSocket)
   {
      UART_WARNING() << "TCP Open call failed:" << SDLNet_GetError();
      return;
   }

   UART_DEBUG() << "Waiting for connection on port " << thePortNumber;

   while(theClientSocket == NULL)
   {
      theClientSocket = SDLNet_TCP_Accept(theServerSocket);

      if (theClientSocket == NULL)
         SDL_Delay(500);
   }
   if (theClientSocket == NULL)
   {
      UART_DEBUG() << "There was an error trying to accept the connection: " << SDLNet_GetError();
   }

}

