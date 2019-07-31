cmake_minimum_required (VERSION 2.6)
project (emu6502)

INCLUDE_DIRECTORIES(./
                    mappers)

set(COMMON_FILES Decoder6502.cpp
                 MemoryController.cpp
                 RomMemory.cpp
                 Cpu6502Defines.cpp
                 Logger.cpp
                 MemoryDev.cpp
                 Utils.cpp
                 Disassembler6502.cpp)

set(DISASS_FILES DisMain.cpp)

set(EMULAT_FILES Cpu6502.cpp
                 DebugServer.cpp
                 DebuggerState.cpp
                 RamMemory.cpp
		 RngDev.cpp
                 EmuMain.cpp
		 MemoryConfig.cpp
		 UartDevice.cpp
		 MirrorMemory.cpp
		 NesRom.cpp
		 cjson/cJSON.c
                 mappers/Mapper.cpp
                 mappers/NRomMapper.cpp
		 SimpleQueue.cpp
		 Display.cpp
		 DisplayDevice.cpp
		 DisplayManager.cpp
		 Easy6502JsDisplay.cpp
		 Easy6502JsInputDevice.cpp )

#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

# PKG_SEARCH_MODULE(SDL2NET REQUIRED SDL2_net>=2.0.0)

add_executable(emu6502 ${COMMON_FILES} ${EMULAT_FILES} )
add_executable(dis6502 ${COMMON_FILES} ${DISASS_FILES} )
INCLUDE(FindPkgConfig)

# Include SDL
find_package(SDL2 REQUIRED)

set(SDL2_net_LIBRARIES "-lSDL2_net")

target_compile_definitions(emu6502 PRIVATE SDL_INCLUDED)

target_link_libraries(emu6502 ${SDL2_LIBRARIES})
target_link_libraries(emu6502 ${SDL2_net_LIBRARIES})

# Modern C++ support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++11 -Wall -D_FORTIFY_SOURCE=2 -O2")

MESSAGE("Compiler flags=${CMAKE_CXX_FLAGS}")



