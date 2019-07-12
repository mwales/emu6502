cmake_minimum_required (VERSION 2.6)
project (emu6502)

# Clang compiler
SET (CMAKE_C_COMPILER             "/usr/bin/clang")
SET (CMAKE_CXX_COMPILER           "/usr/bin/clang++")

# GCC compiler
#SET (CMAKE_C_COMPILER             "/usr/bin/gcc")
#SET (CMAKE_CXX_COMPILER           "/usr/bin/g++")



set(COMMON_FILES ../../src/SimpleQueue.cpp
	         ../../src/Logger.cpp
		 ../../src/Utils.cpp)

set(TESTER_FILES TestMain.cpp)

add_executable(test6502 ${COMMON_FILES} ${TESTER_FILES} )
INCLUDE(FindPkgConfig)

#Include SDL
find_package(SDL2 REQUIRED)
target_link_libraries(test6502 ${SDL2_LIBRARIES})

# Modern C++ support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I../../../src/ --std=c++11 -g -Wall")


MESSAGE("Compiler flags=${CMAKE_CXX_FLAGS}")




