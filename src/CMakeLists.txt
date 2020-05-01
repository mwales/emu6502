cmake_minimum_required (VERSION 2.6)
project (emu6502)

INCLUDE_DIRECTORIES(./
                    devices
		    processors)

set (DEVICE_FILES
	devices/RomMemory.cpp
	devices/RngDev.cpp
	devices/MirrorMemory.cpp
	devices/RamMemory.cpp
	devices/UartDevice.cpp
	devices/NesRom.cpp
	devices/mappers/Mapper.cpp
	devices/mappers/NRomMapper.cpp
	)


set(EMULATOR_FILES  
	EmuMain.cpp
	MemoryFactory.cpp
	ProcessorFactory.cpp
	MemoryController.cpp
	Logger.cpp
	MemoryDev.cpp
	Utils.cpp
	ConfigManager.cpp
	)

add_executable(emu ${EMULATOR_FILES} ${DEVICE_FILES})
INCLUDE(FindPkgConfig)

# Include SDL
find_package(SDL2 REQUIRED)

target_include_directories(emu PUBLIC ${SDL2_INCLUDE_DIRS})

set(SDL2_net_LIBRARIES "-lSDL2_net")

target_link_libraries(emu ${SDL2_LIBRARIES})
target_link_libraries(emu ${SDL2_net_LIBRARIES})

# Modern C++ support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --std=c++11 -Wall -D_FORTIFY_SOURCE=2 -O2")

MESSAGE("Compiler flags=${CMAKE_CXX_FLAGS}")



