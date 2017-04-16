#include "RomMemory.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "Logger.h"

RomMemory::RomMemory(std::string filepath, CpuAddress address):
   theData(0)
{
   theAddress = address;
   theName = "ROM";

   int fd = open(filepath.c_str(), O_RDONLY);

   if (fd <= 0)
   {
      LOG_WARNING() << "ROM INIT ERROR: Couldn't open ROM file " << filepath;
      return;
   }

   int32_t sizeStatus =  lseek(fd, 0, SEEK_END);
   if (sizeStatus < 0)
   {
      LOG_WARNING() << "ROM INIT ERROR: Couldn't seek to the end of the ROM";
      close(fd);
      return;
   }

   if (sizeStatus > UINT16_MAX)
   {
      LOG_WARNING() << "ROM INIT ERROR: ROM File too large for 6502 memory space";
      close(fd);
      return;
   }

   if (sizeStatus > (UINT16_MAX - address))
   {
      LOG_WARNING() << "ROM INIT ERROR: ROM file will not fit in the memory region";
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
         LOG_WARNING() << "ROM INIT ERROR: Error reading the ROM contents";
         close(fd);
         return;
      }

      bytesLeft -= bytesRead;
      bytesReadCumulative += bytesRead;
   }

   LOG_DEBUG() << "ROM INITIALIZED: " << filepath << " (" << theSize << " bytes) "
               << addressToString(theAddress) << "-" << addressToString(theAddress + theSize);
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
   theName = name;
}




uint8_t RomMemory::read8(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr))
   {
      return 0;
   }

   return theData[absAddr - theAddress];
}

bool RomMemory::write8(CpuAddress absAddr, uint8_t val)
{
   if (!isAbsAddressValid(absAddr))
   {
      return false;
   }

   theData[absAddr - theAddress] = val;
   return true;
}

uint16_t RomMemory::read16(CpuAddress absAddr)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return 0;
   }

   uint16_t* retData = (uint16_t*) &theData[absAddr - theAddress];
   return *retData;
}

bool RomMemory::write16(CpuAddress absAddr, uint16_t val)
{
   if (!isAbsAddressValid(absAddr) || !isAbsAddressValid(absAddr + 1))
   {
      return false;
   }

   uint16_t* dataPtr = (uint16_t*) &theData[absAddr - theAddress];
   *dataPtr = val;
   return true;
}


