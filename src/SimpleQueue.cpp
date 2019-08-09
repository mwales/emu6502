#include "SimpleQueue.h"
#include "EmulatorConfig.h"

#include <cstring>

#include "Logger.h"
#include "Utils.h"

#ifdef SIMPLE_QUEUE_LOGGING
   #define SIMPQ_DEBUG   LOG_DEBUG
   #define SIMPQ_WARNING LOG_WARNING
#else
   #define SIMPQ_DEBUG   if(0) LOG_DEBUG
   #define SIMPQ_WARNING if(0) LOG_WARNING
#endif

SimpleQueue::SimpleQueue(int32_t commandQueueSize)
{
   SIMPQ_DEBUG() << "SimpleQueue constructed with size" << commandQueueSize;

   if (!Utils::isPowerOf2(commandQueueSize))
   {
      SIMPQ_WARNING() << "Queue size" << commandQueueSize << "is not power of 2";
      commandQueueSize = Utils::nextPowerOf2(commandQueueSize);
      SIMPQ_DEBUG() << "Next power of 2 size =" << commandQueueSize;
   }

   theMessageBuffer = (char*) calloc(1, commandQueueSize);

   theQueueSize = commandQueueSize;
   theQueueSizeMask = theQueueSize - 1;

   theWritePos = 0;

   theReadPos = 0;

   SDL_AtomicSet(&theNumberOfBytesQueued, 0);
}

SimpleQueue::~SimpleQueue()
{
   SIMPQ_DEBUG() << "SimpleQueue deconstructor";
   free(theMessageBuffer);
}

bool SimpleQueue::writeMessage(int32_t numBytes, char const * const data)
{
   if (numBytes > (theQueueSize - (int32_t) sizeof(int32_t)) )
   {
      // Fatal, command queue isn't big enough for the data trying to be put into it
      SIMPQ_WARNING() << "writeMessage of" << numBytes << "bytes in Q size of" << theQueueSize
                      << "too big for queue";
      return false;
   }

   SIMPQ_DEBUG() << "writeMessage: called with" << numBytes << "bytes.  ReadPos=" <<
                    theReadPos << ", WritePos=" << theWritePos;

   int bytesStoredInQueue = SDL_AtomicGet(&theNumberOfBytesQueued);
   while(bytesStoredInQueue + (int32_t) sizeof(int32_t) + numBytes > theQueueSize)
   {
      SIMPQ_WARNING() << "Waiting for room in Q. Q has" << bytesStoredInQueue
                      << "bytes, command len = " << numBytes;
      SDL_Delay(10);
      bytesStoredInQueue = SDL_AtomicGet(&theNumberOfBytesQueued);
   }

   // There is room in the queue, copy the data into the queue
   // Write the size into the queue
   SIMPQ_DEBUG() << "Writing command size into queue (" << numBytes << ")";
   copyDataIntoQueue(sizeof(int32_t), (char*) &numBytes);

   SIMPQ_DEBUG() << "Writing the data into the queue";
   copyDataIntoQueue(numBytes, data);

   // Let the consumer know there is data ready
   SIMPQ_DEBUG() << "Updating theNumberOfBytesQueued";
   SDL_AtomicAdd(&theNumberOfBytesQueued, sizeof(int32_t) + numBytes);

   return true;
}

bool SimpleQueue::tryWriteMessage(int32_t numBytes, char const * const data)
{
   if (numBytes > (theQueueSize - (int32_t) sizeof(int32_t)) )
   {
      // Fatal, command queue isn't big enough for the data trying to be put into it
      SIMPQ_WARNING() << "tryWriteMessage of" << numBytes << "bytes in Q size of" << theQueueSize
                      << "too big for queue";
      return false;
   }

   int bytesStoredInQueue = SDL_AtomicGet(&theNumberOfBytesQueued);
   if(bytesStoredInQueue + (int32_t) sizeof(int32_t) + numBytes > theQueueSize)
   {
      SIMPQ_WARNING() << "No room in Q. Q has" << bytesStoredInQueue
                      << "bytes, command len = " << numBytes;
      return false;
   }

   SIMPQ_DEBUG() << "tryWriteMessage: called with" << numBytes << "bytes.  ReadPos=" <<
                    theReadPos << ", WritePos=" << theWritePos;

   // There is room in the queue, copy the data into the queue
   // Write the size into the queue
   SIMPQ_DEBUG() << "Writing command size into queue (" << numBytes << ")";
   copyDataIntoQueue(sizeof(int32_t), (char*) &numBytes);

   SIMPQ_DEBUG() << "Writing the data into the queue";
   copyDataIntoQueue(numBytes, data);

   // Let the consumer know there is data ready
   SIMPQ_DEBUG() << "Updating theNumberOfBytesQueued";
   SDL_AtomicAdd(&theNumberOfBytesQueued, sizeof(int32_t) + numBytes);

   return true;
}

int32_t SimpleQueue::readNextMessageSize()
{
   int32_t numBytesInQ = SDL_AtomicGet(&theNumberOfBytesQueued);

   SIMPQ_DEBUG() << "readNextMessageSize: called with ReadPos=" <<
                    theReadPos << ", WritePos=" << theWritePos;

   while(numBytesInQ < (int32_t) sizeof(int32_t))
   {
      SDL_Delay(10);
      numBytesInQ = SDL_AtomicGet(&theNumberOfBytesQueued);
   }

   int32_t retVal;
   readDataFromQueue(sizeof(int32_t), (char*) &retVal, false);

   SIMPQ_DEBUG() << "readNextMessageSize returning" << retVal;
   return retVal;
}

int32_t SimpleQueue::tryReadNextMessageSize()
{
   int32_t numBytesInQ = SDL_AtomicGet(&theNumberOfBytesQueued);

   if(numBytesInQ < (int32_t) sizeof(int32_t))
   {
      // Nothing in the queue yet
      SIMPQ_DEBUG() << "tryReadNextMessageSize returning 0";
      return 0;
   }

   SIMPQ_DEBUG() << "tryReadNextMessageSize: called with ReadPos=" <<
                    theReadPos << ", WritePos=" << theWritePos;

   int32_t retVal;
   readDataFromQueue(sizeof(int32_t), (char*) &retVal, false);

   SIMPQ_DEBUG() << "tryReadNextMessageSize returning" << retVal;
   return retVal;
}

bool SimpleQueue::readMessage(int32_t* numBytes, char* buffer, int bufSize)
{
   int32_t numBytesInQ = SDL_AtomicGet(&theNumberOfBytesQueued);
   SIMPQ_DEBUG() << "readMessage called with" << numBytesInQ << "bytes in queue (first read)";

   while(numBytesInQ < (int32_t) sizeof(int32_t))
   {
      SDL_Delay(10);
      numBytesInQ = SDL_AtomicGet(&theNumberOfBytesQueued);
   }

   SIMPQ_DEBUG() << "readMessage: called with ReadPos=" <<
                    theReadPos << ", WritePos=" << theWritePos;

   // Read the size
   readDataFromQueue(sizeof(int32_t), (char*) numBytes, false);

   // Does the caller have enough room in their buffer?
   if (*numBytes > bufSize)
   {
      SIMPQ_WARNING() << "readMessage has" << *numBytes << "ready, but buffer size is only"
                      << bufSize << "bytes!";
      return false;
   }

   if (*numBytes + (int32_t) sizeof(int32_t) > numBytesInQ)
   {
      SIMPQ_WARNING() << "readMessage failure.  Length stored in queue =" << *numBytes
                      << "bytes, but only " << numBytesInQ << "bytes in queue";
      return false;
   }

   // If we can successfully read the value, update readPos
   SIMPQ_DEBUG() << "theReadPos = (" << theReadPos << "+" << sizeof(int32_t) << ") &"
                 << Utils::toHex32(theQueueSizeMask) << "="
                 << Utils::toHex32( (theReadPos + sizeof(int32_t)) & theQueueSizeMask);
   theReadPos = (theReadPos + sizeof(int32_t)) & theQueueSizeMask;

   readDataFromQueue(*numBytes, buffer, true);
   SIMPQ_DEBUG() << "readMessage read command of" << *numBytes << "bytes";

   // Subtract the bytes read from qty available for reading
   SDL_AtomicAdd(&theNumberOfBytesQueued, -1 * (sizeof(int32_t) + *numBytes));

   return true;
}

bool SimpleQueue::tryReadMessage(int32_t* numBytes, char* buffer, int bufSize)
{
   int32_t numBytesInQ = SDL_AtomicGet(&theNumberOfBytesQueued);

   if(numBytesInQ < (int32_t) sizeof(int32_t))
   {
      SIMPQ_DEBUG() << "tryReadMessage called, but no data in queue";
      *numBytes = 0;
      return true;
   }

   SIMPQ_DEBUG() << "tryReadMessage: called with ReadPos=" <<
                    theReadPos << ", WritePos=" << theWritePos;

   SIMPQ_DEBUG() << "tryReadMessage called with" << numBytesInQ << "bytes in queue";

   // Read the size
   readDataFromQueue(sizeof(int32_t), (char*) numBytes, false);

   // Does the caller have enough room in their buffer?
   if (*numBytes > bufSize)
   {
      SIMPQ_WARNING() << "tryReadMessage has" << *numBytes << "ready, but buffer size is only"
                      << bufSize << "bytes!";
      return false;
   }

   if (*numBytes + (int32_t) sizeof(int32_t) > numBytesInQ)
   {
      SIMPQ_WARNING() << "tryReadMessage failure.  Length stored in queue =" << *numBytes
                      << "bytes, but only " << numBytesInQ << "bytes in queue";
      *numBytes = -1;
      return false;
   }

   // If we can successfully read the value, update readPos
   SIMPQ_DEBUG() << "theReadPos = (" << theReadPos << "+" << sizeof(int32_t) << ") &"
                 << Utils::toHex32(theQueueSizeMask) << "="
                 << Utils::toHex32( (theReadPos + sizeof(int32_t)) & theQueueSizeMask);
   theReadPos = (theReadPos + sizeof(int32_t)) & theQueueSizeMask;

   readDataFromQueue(*numBytes, buffer, true);
   SIMPQ_DEBUG() << "tryReadMessage read command of" << *numBytes << "bytes";

   SDL_AtomicAdd(&theNumberOfBytesQueued, -1 * (sizeof(int32_t) + *numBytes));
   return true;
}

int32_t SimpleQueue::getNumberOfBytesQueued()
{
   int32_t retVal = SDL_AtomicGet(&theNumberOfBytesQueued);

   SIMPQ_DEBUG() << "getNumberOfBytesQueued() returning " << retVal;
   return retVal;
}

void SimpleQueue::copyDataIntoQueue(int32_t numBytes, char const * const buffer)
{
   if (theWritePos + numBytes <= theQueueSize)
   {
      // Simple memcpy case
      SIMPQ_DEBUG() << "WR(whole): pos=" << theWritePos << "=" << Utils::hexDump( (uint8_t*) buffer, (int) numBytes);
      memcpy(theMessageBuffer + theWritePos, buffer, numBytes);
      theWritePos += numBytes;

      // Incase it ends up right at the end
      theWritePos = theWritePos & theQueueSizeMask;
   }
   else
   {
      // Wrapping around the ring case!

      // Copy the end parts
      int part1Size = theQueueSize - theWritePos;
      int part2Size = numBytes - part1Size;

      SIMPQ_DEBUG() << "WR(part1): pos=" << theWritePos << "=" << Utils::hexDump( (uint8_t*) buffer, part1Size);
      memcpy(theMessageBuffer + theWritePos, buffer, part1Size);

      SIMPQ_DEBUG() << "WR(part2): pos= 0 =" << Utils::hexDump( (uint8_t*) (buffer + part1Size), part2Size);
      memcpy(theMessageBuffer, buffer + part1Size, part2Size);

      theWritePos = part2Size;
   }
}

void SimpleQueue::readDataFromQueue(int32_t numBytes, char * const buffer, bool updateReadPos)
{
   if (theReadPos + numBytes <= theQueueSize)
   {
      // Can read everything in one shot
      memcpy(buffer, theMessageBuffer + theReadPos, numBytes);
      SIMPQ_DEBUG() << "RD(whole): pos=" << theReadPos << "=" << Utils::hexDump((uint8_t*) buffer, numBytes);
   }
   else
   {
      // The data wraps back to the beginning of the ring buffer!  2 copies required
      int numBytesFirstRead = theQueueSize - theReadPos;
      int numBytesSecondRead = numBytes - numBytesFirstRead;

      memcpy(buffer, theMessageBuffer + theReadPos, numBytesFirstRead);
      SIMPQ_DEBUG() << "RD(part1): pos=" << theReadPos << "=" << Utils::hexDump((uint8_t*) buffer, numBytesFirstRead);

      memcpy(buffer + numBytesFirstRead, theMessageBuffer, numBytesSecondRead);
      SIMPQ_DEBUG() << "RD(part2): pos= 0 =" << Utils::hexDump((uint8_t*) (buffer + numBytesFirstRead), numBytesSecondRead);
   }

   // Update read position
   if (updateReadPos)
   {
      SIMPQ_DEBUG() << "theReadPos = (" << theReadPos << "+" << numBytes << ") &"
                    << Utils::toHex32(theQueueSizeMask) << "="
                    << Utils::toHex32( (theReadPos + numBytes) & theQueueSizeMask);

      theReadPos = (theReadPos + numBytes) & theQueueSizeMask;
   }
}
