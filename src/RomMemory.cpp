#include "RomMemory.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

RomMemory::RomMemory(std::string filepath, uint16_t address):
   theAddress(address),
   theSize(0),
   theName("ROM"),
   theData(0)
{
   int fd = open(filepath.c_str(), O_RDONLY);

   if (fd <= 0)
   {
      std::cerr << "ROM INIT ERROR: Couldn't open ROM file " << filepath << std::endl;
      return;
   }

   int32_t sizeStatus =  lseek(fd, 0, SEEK_END);
   if (sizeStatus < 0)
   {
      std::cerr << "ROM INIT ERROR: Couldn't seek to the end of the ROM" << std::endl;
      close(fd);
      return;
   }

   if (sizeStatus > UINT16_MAX)
   {
      std::cerr << "ROM INIT ERROR: ROM File too large for 6502 memory space" << std::endl;
      close(fd);
      return;
   }

   if (sizeStatus > (UINT16_MAX - address))
   {
      std::cerr << "ROM INIT ERROR: ROM file will not fit in the memory region" << std::endl;
      close(fd);
      return;
   }

   theSize = sizeStatus;
   theData = new uint8_t[theSize];

   lseek(fd, 0, SEEK_SET);

   uint16_t bytesLeft = theSize;
   int bytesReadCumulative = 0;
   while(bytesLeft)
   {
      int bytesRead = read(fd, &theData[bytesReadCumulative], bytesLeft);

      if (bytesRead <= 0)
      {
         std::cerr << "ROM INIT ERROR: Error reading the ROM contents" << std::endl;
         close(fd);
         return;
      }

      bytesLeft -= bytesRead;
      bytesReadCumulative += bytesRead;
   }

   std::cout << "ROM INITIALIZED: " << filepath << " (" << theSize << " bytes)" << std::endl;
   close(fd);
}

RomMemory::~RomMemory()
{
   if (theData)
   {
      delete[] theData;
      theData = 0;
   }
}

void RomMemory::setName(std::string name)
{

}


std::string RomMemory::getName()
{

}

uint8_t RomMemory::read8(uint16_t offset)
{

}

bool RomMemory::write8(uint16_t offset, uint8_t val)
{

}

uint16_t RomMemory::read16(uint16_t offset)
{

}

bool RomMemory::write16(uint16_t offset, uint16_t val)
{

}

uint16_t RomMemory::getAddress()
{

}
