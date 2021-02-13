#ifndef DISPLAYEVENTS_H
#define DISPLAYEVENTS_H

#include <stdint.h>
#include "SDL.h"

typedef enum
{
   KEY_STATE_CHANGE,
} DisplayEventId;

typedef union
{
   struct
   {
      bool isDown;
      SDL_Keysym keyCode;

   } DeKeyStateChange;



} DisplayEventPayloads;

typedef struct
{
   DisplayEventId id;

   DisplayEventPayloads data;
} DisplayEvent;


#endif // DISPLAYEVENTS_H
