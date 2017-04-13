#pragma once

// Copied from https://github.com/mwales/education/sdl/Asteroids
// Don't make any changes for this project, put them in original project or create a new Logger project in Utils

#include <string>
#include <iostream>
#include <sstream>
#include <ostream>

#define LOG_DEBUG()   Logger(__FILE__, __LINE__)
#define LOG_WARNING() Logger(__FILE__, __LINE__, Logger::LOG_LEVEL_WARNING)
#define LOG_FATAL()   Logger(__FILE__, __LINE__, Logger::LOG_LEVEL_FATAL)

class Logger
{
public:

   enum LogLevel
   {
      LOG_LEVEL_DEBUG,
      LOG_LEVEL_WARNING,
      LOG_LEVEL_FATAL
   };

   Logger(enum LogLevel logLevel = LOG_LEVEL_DEBUG);

   Logger(char const * filename, int lineNumber, enum LogLevel logLevel = LOG_LEVEL_DEBUG);

   ~Logger();

   template<typename T>
   Logger& operator<< (T const & rhs)
   {
      theOss << " " << rhs;
      return *this;
   }

protected:

   static const int FILE_NAME_PRINTED_LENGTH;

   // Logging level
   int theLevel;

   // output stream
   std::ostringstream theOss;

   unsigned int theTimestamp;

};

