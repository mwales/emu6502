REM The default path for MinGW is c:\MinGW
set PATH=%PATH%;c:\MinGW\bin

REM Change this to wherever the SDL2 development libraries are installed
set SDL_PATH=c:\checkouts\sdl2\i686-w64-mingw32\
set SDL_NET_PATH=c:\checkouts\sdl2_net\i686-w64-mingw32\

set SOURCE_FILES=^
Cpu6502.cpp ^
Cpu6502Defines.cpp ^
DebuggerState.cpp ^
DebugServer.cpp ^
Decoder6502.cpp ^
Disassembler6502.cpp ^
Display.cpp ^
DisplayDevice.cpp ^
DisplayManager.cpp ^
Easy6502JsDisplay.cpp ^
Easy6502JsInputDevice.cpp ^
EmuMain.cpp ^
Logger.cpp ^
MemoryConfig.cpp ^
MemoryController.cpp ^
MemoryDev.cpp ^
MirrorMemory.cpp ^
NesRom.cpp ^
RamMemory.cpp ^
RngDev.cpp ^
RomMemory.cpp ^
SimpleQueue.cpp ^
UartDevice.cpp ^
Utils.cpp ^
mappers\NRomMapper.cpp ^
mappers\Mapper.cpp ^
cjson/cJSON.c

REM Order of the libraries at the end are very important
g++ %SOURCE_FILES% -I%SDL_PATH%\include\sdl2 -I%SDL_NET_PATH%\include\SDL2 -Imappers -I.. ^
 -L%SDL_PATH%\lib -L%SDL_NET_PATH%\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_net

REM Keep sdl2.dll in the same directory as your executable to run
copy %SDL_PATH%\bin\sdl2.dll SDL2.dll
copy %SDL_NET_PATH%\bin\sdl2_net.dll SDL2_net.dll






