cmake_minimum_required (VERSION 2.6)
project (emu6502)

set(COMMON_FILES Decoder6502.cpp
                 MemoryController.cpp
                 RomMemory.cpp
                 Cpu6502Defines.cpp
                 Logger.cpp
                 MemoryDev.cpp
                 Utils.cpp)

set(DISASS_FILES Disassembler6502.cpp
                 DisMain.cpp)

set(EMULAT_FILES Cpu6502.cpp
                 EmuMain.cpp)

add_executable(emu6502 ${COMMON_FILES} ${EMULAT_FILES} )
add_executable(dis6502 ${COMMON_FILES} ${DISASS_FILES} )
INCLUDE(FindPkgConfig)

# Modern C++ support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall")


MESSAGE("Compiler flags=${CMAKE_CXX_FLAGS}")




