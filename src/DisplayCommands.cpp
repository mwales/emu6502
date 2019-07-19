#include "DisplayCommands.h"

#define CHECK_COMMAND_SIZE(typeName) if (sizeof(typeName) > retVal) retVal = sizeof(typeName);

uint16_t getSizeOfLargestDisplayCommand()
{
//   uint16_t retVal = 0;

//   CHECK_COMMAND_SIZE(DcHaltEmulation);
//   CHECK_COMMAND_SIZE(DcSetResolution);
//   CHECK_COMMAND_SIZE(DcClearScreen);
//   CHECK_COMMAND_SIZE(DcDrawPixel);

//   return retVal;

   return sizeof(DisplayCommand);


}
