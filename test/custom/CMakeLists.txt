cmake_minimum_required (VERSION 2.6)
project (emu6502)

# Clang compiler
SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_CXX_COMPILER           "/usr/bin/clang++")

# GCC compiler
#SET (CMAKE_C_COMPILER             "/usr/bin/gcc")
#SET (CMAKE_CXX_COMPILER           "/usr/bin/g++")



set(COMMON_FILES ../../src/Decoder6502.cpp
                 ../../src/MemoryController.cpp
                 ../../src/RomMemory.cpp
                 ../../src/Cpu6502Defines.cpp
                 ../../src/Logger.cpp
                 ../../src/MemoryDev.cpp
                 ../../src/Utils.cpp)

set(TESTER_FILES TestMain.cpp)

add_executable(test6502 ${COMMON_FILES} ${TESTER_FILES} )
INCLUDE(FindPkgConfig)

# Modern C++ support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I../../../src/ --std=c++11 -Wall")


MESSAGE("Compiler flags=${CMAKE_CXX_FLAGS}")




