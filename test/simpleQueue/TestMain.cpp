#include <iostream>
#include <string.h>

#define CATCH_CONFIG_MAIN
#include "../catch2/catch.hpp"

#include "SimpleQueue.h"

SDL_Thread* futureThread = nullptr;

int futureFuncDelayMsec = 0;

int futureFunction(void* data);

void setupFuture(int delayMs, int futureIndex)
{
   int* futureData = (int*) malloc(sizeof(int) * 2);

   futureData[0] = delayMs;
   futureData[1] = futureIndex;

   futureThread = SDL_CreateThread(futureFunction, "FF", futureData);
}

void endFuture()
{
   int waitSuccess = 0;
   SDL_WaitThread(futureThread, &waitSuccess);
}

void helloWorldTestHelper()
{
   std::cout << "hello world test helper" << std::endl;
}

SimpleQueue* helperQ;
void queueBytesTestHelper()
{
   std::cout << "queueBytesTestHelper adding 1 byte message to queue" << std::endl;

   char message = 'a';
   helperQ->writeMessage(1, &message);
}

void readBytesTestHelper()
{
   char giantBuf[1000];
   int numBytes = 0;
   helperQ->readMessage(&numBytes, giantBuf, 1000);

   std::cout << "readBytesTestHelper read " << numBytes << "from queue";
}


TEST_CASE("Hello World Test", "[HelloWorld]")
{
   int i = 2;
   i += 4;

   REQUIRE(i == 6);

   std::cout << "Peaches!" << std::endl;

   setupFuture(2000, 0);

   endFuture();

   std::cout << "All done here" << std::endl;
};

TEST_CASE("Verify cant push larger than q size - blocking version", "[error]")
{
   std::cout << "Case: Verify cant push larger than q size - blocking version" << std::endl;

   SimpleQueue uut(100); // which is invalid size, will round up to 128

   char dummyData[200];

   bool success = uut.writeMessage(128, dummyData);
   REQUIRE(success == false);

   success = uut.writeMessage(129, dummyData);
   REQUIRE(success == false);

   success = uut.writeMessage(127, dummyData);
   REQUIRE(success == false);

   success = uut.writeMessage(126, dummyData);
   REQUIRE(success == false);

   success = uut.writeMessage(125, dummyData);
   REQUIRE(success == false);

   success = uut.writeMessage(124, dummyData);
   REQUIRE(success == true);
}

TEST_CASE("Verify basic Q behavior - blocking version", "[verify]")
{
   std::cout << "Case: Verify basic Q behavior - blocking version" << std::endl;

   SimpleQueue uut(100);

   char const* message = "Test message";
   char verifyMessage[100];

   memset(verifyMessage, 0, 100);

   bool success = uut.writeMessage(strlen(message), message);
   REQUIRE(success == true);

   int dataUsed = uut.getNumberOfBytesQueued();

   REQUIRE(dataUsed == strlen(message) + 4);

   // Ask uut how big next message is
   int32_t verifySize = uut.readNextMessageSize();

   REQUIRE(verifySize == strlen(message));

   int32_t verifySize2;
   bool readSuccess = uut.readMessage(&verifySize2, verifyMessage, 100);

   REQUIRE(readSuccess == true);
   REQUIRE(verifySize2 == strlen(message));
   REQUIRE(strcmp(message, verifyMessage) == 0);
}

TEST_CASE("Verify basic Q behavior - non-blocking version", "[verify]")
{
   std::cout << "Case: Verify basic Q behavior - non-blocking version" << std::endl;

   SimpleQueue uut(128);

   // Create a message 01234..
   uint8_t message[256];
   for(int i = 0; i < 256; i++)
   {
      message[i] = (uint8_t) i;
   }

   char verifyMessage[256];

   memset(verifyMessage, 0, 256);

   // Try to write a dumb size
   bool failureCase = uut.tryWriteMessage(130, (char*) message);
   REQUIRE(failureCase == false);

   // Do a good write

   bool success = uut.tryWriteMessage(64, (char*) message);
   REQUIRE(success == true);

   int dataUsed = uut.getNumberOfBytesQueued();

   REQUIRE(dataUsed == 64 + 4);

   // Try to write another one the same size, should fail
   bool success2 = uut.tryWriteMessage(64, (char*) message);
   REQUIRE(success2 == false);

   // Try to write a 3rd message and fill this thing up
   bool success3 = uut.tryWriteMessage(56, (char*) message);
   REQUIRE(success3 == true);

   dataUsed = uut.getNumberOfBytesQueued();
   REQUIRE(dataUsed == 128);

   // Ask uut how big next message is
   int32_t verifySize = uut.readNextMessageSize();

   REQUIRE(verifySize == 64);

   // Ask uut how big next message is with non-blocking
   int32_t verifySizeB = uut.tryReadNextMessageSize();

   REQUIRE(verifySizeB == 64);

   int32_t verifySize2;
   bool readSuccess = uut.readMessage(&verifySize2, verifyMessage, 100);

   REQUIRE(readSuccess == true);
   REQUIRE(verifySize2 == 64);
   REQUIRE(memcmp(message, verifyMessage, 64) == 0);

   bzero(verifyMessage, 256);

   // Read the next message with too small buffer

   int32_t verifySize3;
   bool readSuccess2 = uut.readMessage(&verifySize3, verifyMessage, 32);
   REQUIRE(readSuccess2 == false);
   REQUIRE(verifySize3 == 56);

   // Read the last message successfully
   int32_t verifySize4 = 0;
   bool readSuccess3 = uut.tryReadMessage(&verifySize4, verifyMessage, 56);
   REQUIRE(readSuccess3 == true);
   REQUIRE(verifySize4 == 56);

   // Fail at reading when no message
   int32_t verifySize5 = 0;
   bool readSuccess4 = uut.tryReadMessage(&verifySize5, verifyMessage, 256);
   REQUIRE(readSuccess4 == true);
   REQUIRE(verifySize5 == 0);
}

TEST_CASE("Test blocking behaviors", "[verify]")
{
   std::cout << "Case: Test blocking behaviors" << std::endl;

   // Do a read in the future to unblock our 2nd write
   setupFuture(2000, 2);

   SimpleQueue uut(16);
   helperQ = &uut;

   char testMessage1[] = "Test mesage";

   uut.writeMessage(strlen(testMessage1), testMessage1);

   // Verify it went in
   int32_t sizeStored = uut.getNumberOfBytesQueued();
   REQUIRE(sizeStored == strlen(testMessage1) + sizeof(int32_t));

   char testMessage2[] = "Short";

   // We are going to block on this for about ~2 seconds
   uut.writeMessage(strlen(testMessage2), testMessage2);

   // When we get here, message 1 was popped out by the future
   int32_t sizeStored2 = uut.getNumberOfBytesQueued();
   REQUIRE(sizeStored2 == strlen(testMessage2) + sizeof(int32_t));

   endFuture();

   // Read the short message out
   char verifyShortMsg[10];
   int shortMsgSize;
   uut.readMessage(&shortMsgSize, verifyShortMsg, 10);

   verifyShortMsg[5] = 0;
   std::cout << "Verify msg =" << verifyShortMsg;

   REQUIRE(shortMsgSize == 5);
   REQUIRE(memcmp(verifyShortMsg, testMessage2, 5) == 0);

   verifyShortMsg[5] = 0;
   std::cout << "Verify msg =" << verifyShortMsg;

   // Set future function that will put data in q
   setupFuture(2000,1);

   int32_t emptySize = uut.getNumberOfBytesQueued();
   REQUIRE(emptySize == 0);

   // This read going to block for 2 seconds
   int32_t numBytesRead = 0;
   char testBuffer[10];
   uut.readMessage(&numBytesRead, testBuffer, 10);

   // We should have read 1 bytes
   REQUIRE(numBytesRead == 1);
}


int futureFunction(void* data)
{
   int* futureData = (int*) data;

   std::cout << "Future function delaying " << futureData[0] << "ms" << std::endl;
   SDL_Delay(futureData[0]);

   std::cout << "Calling future function of interest" << std::endl;


   switch(futureData[1])
   {
   case 0:
      helloWorldTestHelper();
      return 0;

   case 1:
      queueBytesTestHelper();
      return 0;

   case 2:
      readBytesTestHelper();
      return 0;

   default:
      return 1;
   }

   free(data);
}

