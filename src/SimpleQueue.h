#ifndef SIMPLEQUEUE_H
#define SIMPLEQUEUE_H

#include <SDL2/SDL.h>
#include <stdint.h>

/**
 * Queue of messages going from lower-level code to the graphics thread.
 *
 * @warning Only 1 thread should push messages, only 1 thread should read messages
 *
 * Queue entries are 32-bit size value, followed by paylaod
 *
 * Queue size must be a power of 2 because it makes the wrap-around calculations a simple logical
 * AND operation instead of modulus operation
 */
class SimpleQueue
{
public:
   /**
    * Constructs the message queue
    * @param messageQueueSize Must be a power of 2
    */
   SimpleQueue(int32_t messageQueueSize);

   ~SimpleQueue();

   /**
    * Writes a message into the queue (Blocking).  The data is copied, and you can free the
    * memory pointed to by data when the call returns
    * @param numBytes Number of bytes to write into the queue
    * @param data Data to write into the queue
    * @return True on success
    */
   bool writeMessage(int32_t numBytes, char const * const data);

   /**
    * Writes a message into the queue (Non-Blocking).  The data is copied, and you can free the
    * memory pointed to by data when the call returns
    * @param numBytes Number of bytes to write into the queue
    * @param data Data to write into the queue
    * @return True on success
    */
   bool tryWriteMessage(int32_t numBytes, char const * const data);

   /**
    * Read the size of the next message.  Blocks until there is a message ready to be read
    * @return Length of the next message that needs to be read
    */
   int32_t readNextMessageSize();

   /**
    * Reads the size of the next available message.  Non-blocking.
    * @return 0 if no message available, size if there is one available
    */
   int32_t tryReadNextMessageSize();

   /**
    * Reads a message from the queue.  This will perform a memcpy
    * @param[out] numBytes Number of bytes read
    * @param buffer Pointer to buffer where message should be copied to
    * @param bufSize Size of buffer
    * @return False if an error occured
    */
   bool readMessage(int32_t* numBytes, char* buffer, int bufSize);

   /**
    * Reads a message from the queue (non-blocking).  This will perform a memcpy
    * @param[out] numBytes Number of bytes read, 0 if no message ready to be read
    * @param buffer Pointer to buffer where message should be copied to
    * @param bufSize Size of buffer
    * @return False if an error occured
    */
   bool tryReadMessage(int32_t* numBytes, char* buffer, int bufSize);

   /**
    * Returns number of bytes of queue that are consumed
    */
   int32_t getNumberOfBytesQueued();

private:

   void copyDataIntoQueue(int32_t numBytes, char const * const buffer);

   void readDataFromQueue(int32_t numBytes, char * const buffer, bool updateReadPos);

   char* theMessageBuffer;

   int32_t theQueueSize;

   int32_t theQueueSizeMask;

   int32_t theWritePos;

   int32_t theReadPos;

   SDL_atomic_t theNumberOfBytesQueued;
};

#endif // SIMPLEQUEUE_H
