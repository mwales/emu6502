#ifndef CHIP8DISPLAY_H
#define CHIP8DISPLAY_H

#include <stdint.h>
#include <vector>

class Display;

class Chip8Display
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
   bool drawSprite(int x, int y, std::vector<uint8_t> spriteData);
   bool drawSuperSprite(int x, int y, std::vector<uint8_t> spriteData);

   bool isHighResMode();

protected:

   Display* theDisplay;




   bool theHighResMode;

   std::vector<std::vector <uint8_t> > thePixels;

   uint8_t thePlaneValue;

   void updateResolution();

};

#endif // CHIP8DISPLAY_H
