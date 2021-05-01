#ifndef CHIP8DISPLAY_H
#define CHIP8DISPLAY_H

#include <stdint.h>
#include <vector>
#include "ISerializable.h"

class Display;

class Chip8Display : public ISerializable
{
public:
   Chip8Display();

   ~Chip8Display();

   void resetDisplay();

   int getScreenWidth();
   int getScreenHeight();
   int getNumBitPlanes();

   void clearScreen();

   // Returns true on collision
   bool drawSprite(unsigned int x, unsigned int y, unsigned int rowsToDraw,
                   std::vector<uint8_t> spriteData);
   bool drawSuperSprite(int x, int y, std::vector<uint8_t> spriteData);

   bool isHighResMode();

   void setResolution(bool highMode);

   void redrawScreen();

   // ISerializable
   virtual uint32_t getSaveStateLength() override;
   virtual bool saveState(uint8_t* buffer, uint32_t* bytesSaved) override;
   virtual bool loadState(uint8_t* buffer, uint32_t* bytesLoaded) override;

protected:

   /**
    * Gets the mask for a single plane.  Indexes the list of planes that are
    * currently turned on (index should be less than getNumBitPlanes())
    */
   uint8_t getPlaneMask(int index);

   Display* theDisplay;

   bool theHighResMode;

   std::vector<std::vector <uint8_t> > thePixels;

   uint8_t thePlaneValue;

   void updateResolution();

};

#endif // CHIP8DISPLAY_H