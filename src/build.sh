#!/bin/bash

clang++ --std=c++11 Logger.cpp Disassembler6502.cpp Decoder6502.cpp MemoryDev.cpp RomMemory.cpp MemoryController.cpp Utils.cpp dis.cpp -o dis6502
# g++ --std=c++11 Logger.cpp RomMemory.cpp MemoryDev.cpp emu.cpp -o emu6502

