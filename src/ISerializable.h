#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H

#include <stdint.h>

class ISerializable
{
public:

   /**
    * @return Number of bytes required to save device state
    */
   virtual uint32_t getSaveStateLength() = 0;

   /**
    * Saves the state of the device into the buffer provided
    *
    * @param buffer Where to save the data
    * @param[out] bytesSaved How many bytes were saved.  0 if the device doesn't save state.
    * @return False on save error, true if successful or no state to save.
    */
   virtual bool saveState(uint8_t* buffer, uint32_t* bytesSaved) = 0;

   /**
    * Loads state of the device from a file
    *
    * @param buffer Where to load the data from
    * @param bytesLoaded[out] How many bytes were loaded from
    * @return False on load error, true if successful or no state to save
    */
   virtual bool loadState(uint8_t* buffer, uint32_t* bytesLoaded) = 0;

};

#endif // ISERIALIZABLE_H
