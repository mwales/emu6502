#!/bin/bash

crasm opCodes.asm -o opCodes.srec
objcopy -I srec -O binary opCodes.srec opCodes.bin

