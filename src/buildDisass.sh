#!/bin/bash

# Simple build for the disassembler using C++ 11
clang++ --std=c++11 -g Logger.cpp Cpu6502Defines.cpp Disassembler6502.cpp Decoder6502.cpp MemoryDev.cpp RomMemory.cpp MemoryController.cpp Utils.cpp DisMain.cpp -o dis6502
# g++ --std=c++11 Logger.cpp RomMemory.cpp MemoryDev.cpp emu.cpp -o emu6502

