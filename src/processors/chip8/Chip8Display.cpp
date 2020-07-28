#include "Chip8Display.h"
#include "Display.h"

#define HIGH_RES_WIDTH     128
#define HIGH_RES_HEIGHT    64
#define LOW_RES_WIDTH      64
#define LOW_RES_HEIGHT     32

Chip8Display::Chip8Display()
{

}

Chip8Display::~Chip8Display()
{

}

void Chip8Display::resetDisplay()
{
   theDisplay = Display::getInstance();

   theHighResMode = false;

   // Set the resolution of the SDL display
   theDisplay->setResolution(HIGH_RES_WIDTH * 10, HIGH_RES_HEIGHT * 10);
   theDisplay->setLogicalSize(getScreenWidth(), getScreenHeight());


   updateResolution();

   // populate the pixels
   std::vector<uint8_t> rowOfPixels;
   for(int x = 0; x < HIGH_RES_WIDTH; x++)
   {
      rowOfPixels.push_back(0);
   }

   for(int y = 0; y < HIGH_RES_HEIGHT; y++)
   {
      thePixels.push_back(rowOfPixels);
   }

   // Defaut the XO chip drawing plane for backwards compatibility
   thePlaneValue = 1;

   clearScreen();

}

void Chip8Display::clearScreen()
{
   Color24 white = {0, 0, 0};
   theDisplay->clearScreen(white);

   for(int y = 0; y < HIGH_RES_HEIGHT; y++)
   {
      for(int x = 0; x < HIGH_RES_WIDTH; x++)
      {
         thePixels[y][x] = 0;
      }
   }


}

int Chip8Display::getScreenWidth()
{
   return (theHighResMode ? HIGH_RES_WIDTH : LOW_RES_WIDTH);
}

int Chip8Display::getScreenHeight()
{
   return (theHighResMode ? HIGH_RES_HEIGHT : LOW_RES_HEIGHT);
}

int Chip8Display::getNumBitPlanes()
{
   uint8_t curBit = 1;
   int bitCount = 0;
   for(int i = 0; i < 7; i++)
   {
      if (thePlaneValue & curBit)
      {
         bitCount++;
      }

      curBit <<= 1;
   }

   return bitCount;
}


bool Chip8Display::drawSprite(int x, int y, std::vector<std::uint8_t> spriteData)
{
   return true;
}

bool Chip8Display::drawSuperSprite(int x, int y, std::vector<std::uint8_t> spriteData)
{
   return true;
}

bool Chip8Display::isHighResMode()
{
   return theHighResMode;
}

void Chip8Display::updateResolution()
{
   if (theHighResMode)
   {
      theDisplay->setLogicalSize(HIGH_RES_WIDTH, HIGH_RES_HEIGHT);
   }
   else
   {
      theDisplay->setLogicalSize(LOW_RES_WIDTH, LOW_RES_HEIGHT);
   }
}
