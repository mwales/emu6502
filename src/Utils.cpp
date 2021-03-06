#include "Utils.h"

#include <sstream>

Utils::Utils()
{

}

std::string Utils::toHexNibble(uint8_t val)
{
   char buf[5];
   sprintf(buf, "%x", val & 0x0f);
   return buf;
}

std::string Utils::toHex8(uint8_t val, bool withPrefix)
{
   char buf[5];

   if (withPrefix)
      snprintf(buf, 5, "0x%02x", val);
   else
      snprintf(buf, 5, "%02x", val);

   buf[4] = 0;

   return buf;
}


std::string Utils::toHex16(uint16_t val, bool withPrefix)
{
   char buf[7];

   if(withPrefix)
      snprintf(buf, 7, "0x%04x", val);
   else
      snprintf(buf, 7, "%04x", val);

   buf[6] = 0;

   return buf;
}

std::string Utils::toHex32(uint32_t val, bool withPrefix)
{
   char buf[11];

   if(withPrefix)
      snprintf(buf, 11, "0x%08x", val);
   else
      snprintf(buf, 11, "%08x", val);
   buf[10] = 0;

   return buf;
}

std::string Utils::toHex64(uint64_t val, bool withPrefix)
{
   char buf[19];

   if(withPrefix)
      snprintf(buf, 19, "0x%016lx", val);
   else
      snprintf(buf, 19, "%016lx", val);
   buf[18] = 0;

   return buf;
}


uint8_t Utils::parseUInt8(std::string userInput, bool* success)
{
   uint32_t val = parseUInt32(userInput, success);
   if ( (val > UINT8_MAX) && (success != nullptr) )
   {
     *success = false;
   }
   return (uint8_t) val;
}

uint16_t Utils::parseUInt16(std::string userInput, bool* success)
{
   uint32_t val = parseUInt32(userInput, success);
   if ( (val > UINT16_MAX) && (success != nullptr) )
   {
     *success = false;
   }
   return (uint16_t) val;
}

uint32_t Utils::parseUInt32(std::string userInput, bool* success)
{
   errno = 0;
   uint32_t val = 0;
   char const * userInputCharStar = userInput.c_str();

   if ( (userInput.size() > 2) &&
        (userInput[0] == '0') &&
        (userInput[1] == 'x') )
   {
      // User provided base 16 number
      val = strtoul(userInputCharStar + 2, 0, 16);
   }
   else
   {
      // User provided base 16 number
      val = strtoul(userInputCharStar, 0, 10);
   }

   if ( (errno != 0) && (success != nullptr) )
   {
      *success = false;
   }
   else if (success != nullptr)
   {
      *success = true;
   }

   return val;
}

uint64_t Utils::parseUInt64(std::string userInput, bool* success)
{
   errno = 0;
   uint64_t val = 0;
   char const * userInputCharStar = userInput.c_str();

   if ( (userInput.size() > 2) &&
        (userInput[0] == '0') &&
        (userInput[1] == 'x') )
   {
      // User provided base 16 number
      val = strtoull(userInputCharStar + 2, 0, 16);
   }
   else
   {
      // User provided base 16 number
      val = strtoull(userInputCharStar, 0, 10);
   }

   if ( (errno != 0) && (success != nullptr) )
   {
      *success = false;
   }
   else if (success != nullptr)
   {
      *success = true;
   }

   return val;
}




std::string Utils::loadFile(std::string& name, std::string& errorOut)
{
   std::string retVal;

   SDL_RWops* f = SDL_RWFromFile(name.c_str(), "r");

   if (f == NULL)
   {
      errorOut = SDL_GetError();
      return retVal;
   }

   int fileSize = SDL_RWsize(f);
   if (fileSize == -1)
   {
      errorOut = "Error getting the file size: ";
      errorOut += SDL_GetError();
      SDL_RWclose(f);
      return retVal;
   }

   retVal.reserve(fileSize);

   char buf[4097];
   int bytesToRead = fileSize;
   while(bytesToRead)
   {
      int bytesToReadIntoBuf = 4096;
      if ( bytesToReadIntoBuf > bytesToRead)
         bytesToReadIntoBuf = bytesToRead;

      int numBytes = SDL_RWread(f, buf, 1, bytesToReadIntoBuf);

      if (numBytes == 0)
         break;

      buf[numBytes] = 0;
      retVal += buf;

      bytesToRead -= numBytes;
   }

   SDL_RWclose(f);
   return retVal;
}

std::vector<uint8_t> Utils::loadFileBytes(std::string const & name, std::string& errorOut)
{
   std::vector<uint8_t> retVal;

   SDL_RWops* f = SDL_RWFromFile(name.c_str(), "r");

   if (f == NULL)
   {
      errorOut = SDL_GetError();
      return retVal;
   }

   int fileSize = SDL_RWsize(f);
   if (fileSize == -1)
   {
      errorOut = "Error getting the file size: ";
      errorOut += SDL_GetError();
      SDL_RWclose(f);
      return retVal;
   }

   retVal.reserve(fileSize);

   uint8_t buf[4096];
   int bytesToRead = fileSize;
   while(bytesToRead)
   {
      int bytesToReadIntoBuf = 4096;
      if ( bytesToReadIntoBuf > bytesToRead)
         bytesToReadIntoBuf = bytesToRead;

      int numBytes = SDL_RWread(f, buf, 1, bytesToReadIntoBuf);

      if (numBytes == 0)
         break;

      buf[numBytes] = 0;
      for(int i = 0; i < numBytes; i++)
      {
         retVal.push_back(buf[i]);
      }
      
      bytesToRead -= numBytes;
   }

   SDL_RWclose(f);
   return retVal;
}

std::string Utils::saveFile(std::string const & filename, uint8_t* buf, int fileLen)
{
   SDL_RWops* f = SDL_RWFromFile(filename.c_str(), "w+");

   if (f == NULL)
   {
      return "Error opening file for writing";
   }

   int objsWritten = SDL_RWwrite(f, buf, fileLen, 1);

   SDL_RWclose(f);

   if (objsWritten != 1)
   {
      return "Error writing the file";
   }
   else
   {
      // Success
      return "";
   }
}

std::vector<std::string> Utils::tokenizeString(std::string const & input)
{
   std::vector<std::string> retVal;
   int startToken = -1;

   for(unsigned int i = 0; i < input.size(); i++)
   {
      char curChar = input.at(i);
      if ( (curChar == ' ') || (curChar == '\t') || (curChar == '\n') || (curChar == '\r'))
      {
         // Found whitespace
         if (startToken != -1)
         {
            // Found a token
            std::string token = input.substr(startToken, i - startToken);
            retVal.push_back(token);

            startToken = -1;
         }
      }
      else
      {
         // Not whitespace
         if (startToken == -1)
         {
            startToken = i;
         }
      }
   }

   return retVal;
}

int Utils::readUntilEof(uint8_t* buffer, int numBytes, SDL_RWops* fp)
{
    int bytesReadTotal = 0;
    while (bytesReadTotal < numBytes)
    {
        int bytesRead = SDL_RWread(fp, buffer + bytesReadTotal, 1, numBytes - bytesReadTotal);

        if (bytesRead == 0)
        {
            // We are out of bytes to read
            return bytesReadTotal;
        }

        bytesReadTotal += bytesRead;
    }

    return bytesReadTotal;
}


std::string Utils::hexDump(uint8_t* buffer, int length)
{
   std::ostringstream oss;
   bool isFirst = true;
   for(int i = 0; i < length; i++)
   {
      if (isFirst)
      {
         isFirst = false;
      }
      else
      {
         oss << " ";
      }

      oss << toHex8(buffer[i], false);
   }

   return oss.str();
}

std::string Utils::hexDumpMemory(uint8_t* buffer, uint32_t length,
                                 uint32_t address, uint32_t addrBytes, bool asciiToo)
{
   std::string retVal;
   uint32_t curAddress = address;
   int64_t bytesLeft = length;
   while(bytesLeft > 0)
   {
      retVal += hexDumpMemorySingleLine(buffer, bytesLeft,
                                        curAddress, addrBytes, asciiToo);

      int bytesToAdd = 0x10 - (curAddress & 0xf);

      bytesLeft -= bytesToAdd;
      curAddress += bytesToAdd;
   }

   return retVal;
}

std::string Utils::hexDumpMemorySingleLine(uint8_t* buffer, uint32_t length,
                                           uint32_t address, uint32_t addrBytes,
                                           bool asciiToo)
{
   std::string retVal;
   std::string asciiDump = "|";

   uint32_t startCol = address & 0xf;

   uint32_t endCol = 0xf;
   if (startCol + length < 0x10)
   {
      endCol = startCol + length;
   }

   // Write the address portion of the memory dump
   if (addrBytes == 8)
   {
      retVal += toHex8(address & 0xf0);
   }
   else if (addrBytes == 16)
   {
      retVal += toHex16(address & 0xfff0);
   }
   else
   {
      retVal += toHex32(address & 0xfffffff0);
   }

   // Write the spacing before the hex data starts
   for (uint32_t i = 0; i < startCol; i++)
   {
      retVal += "   ";
      asciiDump += " ";
   }

   // Write the hex bytes
   for(uint32_t i = startCol; i < endCol; i++)
   {
      retVal += hexDump(buffer + i, 1);
      retVal += " ";

      if ( (buffer[i] < 0x20) || (buffer[i] > 0x7e) )
      {
         asciiDump += ".";
      }
      else
      {
         asciiDump += (char) buffer[i];
      }
   }

   // Write the trailing spaces
   for(uint32_t i = endCol; i < 0x10; i++)
   {
      retVal += "   ";
      asciiDump += " ";
   }

   asciiDump += "|";

   retVal += "  ";
   retVal += asciiDump;

   return retVal;
}

bool Utils::isPowerOf2(int32_t val)
{
   return ( (val & (val - 1)) == 0);
}

int32_t Utils::nextPowerOf2(int32_t v)
{
   // From bithacks page
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
   v++;
   return v;
}

std::vector<std::string> Utils::tokenizeString(std::string const & text, char delimiter)
{
   std::vector<std::string> retVal;
   
   std::string curToken;
   for(auto singleChar: text)
   {
      if (singleChar == delimiter)
      {
         // Found a delimter
         if (curToken.size() > 0)
         {
            retVal.push_back(curToken);
         }
         
         curToken = "";
         continue;
      }
      
      curToken += singleChar;
   }
   
   if (curToken.size() > 0)
   {
      retVal.push_back(curToken);
   }
   
   return retVal;
}
