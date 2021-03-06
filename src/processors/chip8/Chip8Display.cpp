#include "Chip8Display.h"
#include "Display.h"
#include "Logger.h"

#ifdef CHIP8_DISPLAY_DEBUG
   #define C8DISPDEBUG  LOG_DEBUG
   #define C8DISPWARN  LOG_WARNING
#else
   #define C8DISPDEBUG    if(0) LOG_DEBUG
   #define C8DISPWARN  if(0) LOG_WARNING
#endif


#define HIGH_RES_WIDTH     128
#define HIGH_RES_HEIGHT    64
#define LOW_RES_WIDTH      64
#define LOW_RES_HEIGHT     32

Color24 white = {255, 255, 255};
Color24 black = {0, 0, 0};

Chip8Display::Chip8Display()
{

}

Chip8Display::~Chip8Display()
{

}

void Chip8Display::resetDisplay()
{
   C8DISPDEBUG() << "Resetting display";

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
   C8DISPDEBUG() << "Clear screen";

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


bool Chip8Display::drawSprite(unsigned int x, unsigned int y, unsigned int rowsToDraw,
                              std::vector<std::uint8_t> spriteData)
{
   // Update the frame buffer / detect collision, then redraw the data
   C8DISPDEBUG() << "drawSprite @ (" << x << "," << y << "), rows = " << rowsToDraw
                 << ", and numBytes=" << spriteData.size();

   bool collisionFlag = false;
   unsigned int spriteDataByte = 0;

   for (int curPlaneIndex = 0; curPlaneIndex < getNumBitPlanes(); curPlaneIndex++)
   {
      uint8_t curPlaneMask = getPlaneMask(curPlaneIndex);

      for(unsigned int rowOffset = 0; rowOffset < rowsToDraw; rowOffset++)
      {
         // Make sure we are not reading past the end of the sprite data
         if (spriteDataByte >= spriteData.size())
         {
            C8DISPWARN() << "drawSprite requires more data bytes in sprite data";
         }
         uint8_t spriteDataRow = spriteData[spriteDataByte];

         C8DISPDEBUG() << "Current Row Data = " << Utils::toHex8(spriteDataRow);

         uint8_t curPixel = 0x80;
         for(int i = 0; i < 8; i++)
         {

            // Are we setting this pixel?
            if (spriteDataRow & curPixel)
            {
               uint8_t oldValue = thePixels[y+rowOffset][x + i];
               if (oldValue & curPlaneMask)
               {
                  // Collision!
                  collisionFlag = true;
               }

               thePixels[y+rowOffset][x+i] = oldValue ^ curPlaneMask;

            }

            curPixel >>= 1;
         }

         spriteDataByte += 1;
      } // rowOffset loop

   } // curPlaneIndex loop

   // After we update the frame buffer, update the display
   for(unsigned int curX = x; curX < x + 8; curX++)
   {
      for(unsigned int curY = y; curY < y + rowsToDraw; curY++)
      {
         /// @todo Update for XO Chip
         if (thePixels[curY][curX])
         {
            // Draw an "on" pixel
            theDisplay->drawPixel(curX, curY, white);
         }
         else
         {
            theDisplay->drawPixel(curX, curY, black);
         }
      }
   }

   // After updating all the sprite pixels, then render the screen
   theDisplay->render();
   return collisionFlag;
}

bool Chip8Display::drawSuperSprite(int x, int y, std::vector<std::uint8_t> spriteData)
{
   return true;
}

bool Chip8Display::isHighResMode()
{
   return theHighResMode;
}

void Chip8Display::setResolution(bool highMode)
{
   // Is the resolution already set to this?
   if (highMode == theHighResMode)
   {
      C8DISPDEBUG() << "setResolution(" << (theHighResMode ? "high" : "low")
                    << ") = already set!";
      return;
   }

   C8DISPDEBUG() << "setResolution(" << (theHighResMode ? "high" : "low") << ")";
   if (highMode)
   {
      theDisplay->setResolution(HIGH_RES_WIDTH, HIGH_RES_HEIGHT);
   }
   else
   {
      theDisplay->setResolution(LOW_RES_WIDTH, LOW_RES_HEIGHT);
   }

   theHighResMode = highMode;

}

void Chip8Display::redrawScreen()
{
   C8DISPDEBUG() << "redrawScreen()";

   for(int x = 0; x < getScreenWidth(); x++)
   {
      for(int y = 0; y < getScreenHeight(); y++)
      {
         if (thePixels[y][x])
         {
            theDisplay->drawPixel(x, y, white);
         }
         else
         {
            theDisplay->drawPixel(x, y, black);
         }
      }
   }
}

uint8_t Chip8Display::getPlaneMask(int index)
{
   int curIndex = 0;
   uint8_t curPlane = 1;
   for(int i = 0; i < 8; i++)
   {
      curPlane = 1 << i;
      if (curPlane & thePlaneValue)
      {
         // This plane is turned on
         if (index == curIndex)
         {
            return curPlane;
         }
         else
         {
            curIndex += 1;
         }
      }
   }

   // If you get this far without finding the plane, you done screwed up
   C8DISPWARN() << "getPlaneMask(" << index << ") failed, not enough planes set";
   return 0;
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

typedef struct C8DisplayState
{
   uint32_t theMagicNumber;
   uint8_t pixelData[HIGH_RES_HEIGHT][HIGH_RES_WIDTH];
   uint8_t theMode;
   uint8_t thePlanes;
} C8DisplayState;

#define DISPLAY_MAGIC_NUMBER 0xd1591a45

uint32_t Chip8Display::getSaveStateLength()
{
   return sizeof(C8DisplayState);
}

bool Chip8Display::saveState(uint8_t* buffer, uint32_t* bytesSaved)
{
   C8DISPDEBUG() << "saveState()";

   C8DisplayState state;
   memset(&state, 0, sizeof(C8DisplayState));

   for(int y = 0; y < HIGH_RES_HEIGHT; y++)
   {
      for(int x = 0; x < HIGH_RES_WIDTH; x++)
      {
         state.pixelData[y][x] = thePixels[y][x];

      }
   }

   state.theMagicNumber = DISPLAY_MAGIC_NUMBER;
   state.theMode = (theHighResMode ? 1 : 0);
   state.thePlanes = thePlaneValue;

   memcpy(buffer, &state, sizeof(C8DisplayState));

   *bytesSaved = sizeof(C8DisplayState);
   return true;
}

bool Chip8Display::loadState(uint8_t* buffer, uint32_t* bytesLoaded)
{
   C8DISPDEBUG() << "loadState()";

   C8DisplayState state;
   memcpy(&state, buffer, sizeof(C8DisplayState));

   if (state.theMagicNumber != DISPLAY_MAGIC_NUMBER)
   {
      std::cout << "Error reading display data (invalid magic number)" << std::endl;
      return false;
   }

   for(int y = 0; y < HIGH_RES_HEIGHT; y++)
   {
      for(int x = 0; x < HIGH_RES_WIDTH; x++)
      {
         thePixels[y][x] = state.pixelData[y][x];

         if (state.pixelData[y][x] == 1)
         {
            theDisplay->drawPixel(x, y, white);
         }
         else
         {
            theDisplay->drawPixel(x, y, black);
         }
      }
   }

   setResolution(state.theMode);
   thePlaneValue = state.thePlanes;

   *bytesLoaded = sizeof(C8DisplayState);

   theDisplay->render();

   return true;
}
