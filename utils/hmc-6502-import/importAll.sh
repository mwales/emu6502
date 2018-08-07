#!/bin/bash

# Converts all the HMC test rom files into binary files that I can load into
# my emulator.  The output files are put in the testbins directory

if [ $# -ne 1 ];
then
	echo "Usage: ${0} romDir"
	exit 1
fi

if [ ! -d testbin ];
then
	echo "Creating the testbin directory"
	mkdir testbin
else
	echo "testbin directory already exists"
fi

echo "Searching ${1} for .rom files"

find ${1} -name "*.rom" -exec ./hjrom-import.py {} {}.bin \; 

echo "Moving all the generated bins to the testbin directory"

find ${1} -name "*.rom.bin" -exec mv {} testbin \;

