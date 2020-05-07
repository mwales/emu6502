#ifndef DEBUGGER_H
#define DEBUGGER_H


#include <string>
#include <vector>
#include "SimpleMap.h"

class MemoryController;

typedef void (*DebugCommandHandler)(std::vector<std::string> const & args, void* context);

typedef struct
{
   DebugCommandHandler theHandlerFunc;
   std::string theHelp;
   void* theContext;
} HandlerData;

class Debugger
{
public:
   
    Debugger(MemoryController* mc);
    
    void start();
    
    void registerNewCommandHandler(std::string commandName, 
                                   std::string shortHelp,
                                   DebugCommandHandler handler,
                                   void* context);
    
    
private:
    
    MemoryController* theMC;
    
    SimpleMap<std::string, HandlerData> theCommandHandlers;
    
    static void helpCommandHandlerStatic(std::vector<std::string> const & args, 
                                         void* context);
    void helpCommandHandler(std::vector<std::string> const & args);
    
};



#endif
