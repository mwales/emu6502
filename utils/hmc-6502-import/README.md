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

```
./hjrom-import.py testRom.rom output.bin
```

A file named ignore.txt is output with all of the comment lines.

== Import All

I create a script to import all the SuiteA test rom from the HMC
(Harvey Mudd College) repo.

Once all the test roms are in the testbin folder, I will create
unit tests / verification tests to run against the ROMs as I 
update and refactor the emulator.

== Emulator execution tests

After running through each of the HMC test roms, verifying the results
with as much scrutiny as possible, the test output is moved into the
expectedResults folder.  A test case will then be added as the emulator
improves.  Future additions will can then easily be verified not to 
break the old functionality of the emulator.

To run the tests:

```
./run_tests.sh
```


