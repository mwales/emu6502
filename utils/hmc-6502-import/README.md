There was a git repo on google code that has been imported by like
a million people on github.  It has some 6502 test suites written
by Heather Justice.  The ROM files they give you have hexadecimal
ROM contents surrounded by C style comments (explaining what they
do and what the results should be).  This will compile the ROM into
a binary file.

The particular repo I got the test roms from:

https://github.com/fromGoogleCode/hmc-6502

== How to use

The input files are in the test/roms/SuiteA directory

./hjrom-import.py testRom.rom output.bin

A file named ignore.txt is output with all of the comment lines.


