#ifndef DEBUGGER_H
#define DEBUGGER_H


#include <string>
#include <vector>
#include "SimpleMap.h"

/**
 * Macro that generates a simple function that looks like the following
 * void g_stepCommandHandler(std::vector<std::string> const & args, void* context)
 * {
 *    Processor* p = reinterpret_cast<Processor*>(context);
 *    p->stepCommandHandler(args);
 * }
*/

#define DECLARE_DEBUGGER_CALLBACK(classType, callbackName) \
   void g_##callbackName(std::vector<std::string> const & args, void* context) \
   { \
     classType* instance = reinterpret_cast<classType*>(context); \
     instance->callbackName(args); \
   }


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
    
    void helpCommandHandler(std::vector<std::string> const & args);


private:
    
    MemoryController* theMC;
    
    SimpleMap<std::string, HandlerData> theCommandHandlers;
    

    
};



#endif
