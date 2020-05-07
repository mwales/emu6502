#include "Debugger.h"
#include "MemoryController.h"

#include <SDL.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

#include "Utils.h"


Debugger::Debugger(MemoryController* mc)
{
   theMC = mc;
   
   DebugCommandHandler dch = &Debugger::helpCommandHandlerStatic;
   
   registerNewCommandHandler("help", "Lists all the commands available", dch, (void*) this);
}

void Debugger::start()
{
   while(true)
   {
      char* data = readline("emu> ");
      
      std::vector<std::string> stringParts = Utils::tokenizeString(data, ' ');
      
      if (stringParts.size() == 0)
      {
         // No commands entered
         continue;
      }
      
      std::string command = stringParts[0];
      stringParts.erase(stringParts.begin());
      
      if (command == "quit")
      {
         std::cout << "Quiting..." << std::endl;
         break;
      }
      
      // Do we have a command handler registered
      if (theCommandHandlers.contains(command))
      {
         HandlerData hd = theCommandHandlers.getValue(command);
         hd.theHandlerFunc(stringParts, hd.theContext);
      }
      else
      {
         std::cout << "No handler found for command: " << command << std::endl;
      }
      
      add_history(data);
      free(data);
      
   }
   
   std::cout << "Done" << std::endl;
}

void Debugger::registerNewCommandHandler(std::string commandName, 
                                         std::string helpString,
                                         DebugCommandHandler handler,
                                         void* context)
{
   HandlerData hd;
   hd.theHandlerFunc = handler;
   hd.theHelp = helpString;
   hd.theContext = context;
   theCommandHandlers.add(commandName, hd);
}

void Debugger::helpCommandHandlerStatic(std::vector<std::string> const & args, void* context)
{
   Debugger* d = reinterpret_cast<Debugger*>(context);
   d->helpCommandHandler(args);
}

void Debugger::helpCommandHandler(std::vector<std::string> const & args)
{
   std::cout << "Command List:" << std::endl;
   std::cout << "  quit: Exits emulator" << std::endl;
   for(auto singleCommand: theCommandHandlers.getKeys())
   {
      std::cout << "  " << singleCommand << ": " <<  theCommandHandlers.getValue(singleCommand).theHelp << std::endl;
   }
}

