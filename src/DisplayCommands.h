#ifndef DISPLAY_COMMANDS_H
#define DISPLAY_COMMANDS_H

#include <stdint.h>

typedef struct
{
   uint8_t red;
   uint8_t green;
   uint8_t blue;
} Color24;

typedef enum
{
   HALT_EMULATION,
   SET_RESOLUTION,
   SET_LOGICAL_SIZE,
   CLEAR_SCREEN,
   DRAW_PIXEL,
   SUBSCRIBE_SDL_EVENT_TYPE
} DisplayCommandId;

typedef union
{

   struct
   {
      // DisplayCommands id;
      // No parameters required
   } DcHaltEmulation;

   struct
   {
      // DisplayCommands id;
      uint16_t width;
      uint16_t height;
   } DcSetResolution;

   struct
   {
      uint16_t width;
      uint16_t height;
   } DcSetLogicalSize;

   struct
   {
      // DisplayCommands id;
      Color24 blankColor;
   } DcClearScreen;

   struct
   {
      // DisplayCommands id;
      uint16_t x;
      uint16_t y;
      Color24 color;
   } DcDrawPixel;

   struct
   {
       uint32_t eventType;
   } DcSubscribeSdlEventType;

} DisplayCommandPayloads;

typedef struct
{
   DisplayCommandId id;

   DisplayCommandPayloads data;
} DisplayCommand;

#endif

