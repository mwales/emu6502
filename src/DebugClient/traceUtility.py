#!/usr/bin/env python

from dbgClient import *
import sys
import subprocess
import time
import re

def printUsage(progName):
    print("This utility will trace the execution of a binary in the 6502 emulator by printing all registters")
    print("after each instruction is executed")
    print("")
    print("{} binaryFile loadAddressHex [-t=numStepsTraceDec] [-d=f000]").format(progName)
    print("{} --config   config.json    [-t=numStepsTraceDec] [-d=f000]").format(progName)
    print("")
    print("You must provide a binary and address to load the binary.  You can optionally specify the number of")
    print("steps you want to trace.  You can also specify some memory pages you want dumped at the end of")
    print("trace execution")
    print("")

def main(args):
    if (len(args) < 3):
        printUsage(args[0])
        return

    if ( args[1] == "--config" ):
        configFile = args[2]
        spawnArgs = [ './emu6502', "-c", configFile, "-d", "6502" ]
    else:
        binaryFile = args[1]
        loadAddress = int(args[2], 16)
        spawnArgs = [ './emu6502', "-f", binaryFile, "-b", hex(loadAddress), "-d", "6502", "-c", "../configs/emudev.json"]

    if ( "--profiling" in args ):
        print ("Running with profiling")
        # Want to add one of the following options
        # valgrind --tool=callgrind -v --dump-every-bb=10000
        # perf record -a -g -F 97
        profilingPrefix = "perf record -a -g -F 97"
        pArgs = profilingPrefix.split()

        for singlePArg in reversed(pArgs):
            spawnArgs.insert(0, singlePArg)

    print("Emulator cmd line={}".format(" ".join(spawnArgs)))

    
    dumpList = []
    numSteps = -1
    if (len(args) >= 4):
        for argNum in range(3,len(args)):
            curArg = args[argNum]

            if curArg.startswith("-t="):
                # User specified number of trace steps
                numSteps = int(curArg[3:])
                print("Tracing {} steps".format(numSteps))

            if curArg.startswith("-d="):
                # User specified they want memory dumped at the end of execution
                dumpAddr = int(curArg[3:], 16)
                print("Dumping address {} at the end of trace execution".format(hex(dumpAddr)))
                dumpList.append(dumpAddr)


    print("Going to spawn the emulator process")
    
    emulatorStdOut = open("emulator_stdout.txt", "w+")
    emulatorStdErr = open("emulator_stderr.txt", "w+")

    traceOutput = open("trace_output.txt", "w+")

    # Start the emulator in an external process
    p = subprocess.Popen(spawnArgs, stdout=emulatorStdOut, stderr=emulatorStdErr)

    # Wait a second for the emulator to get setup and running
    time.sleep(2)

    # Connect to the emulator via the debugging interface
    dc = DbgClient()
    dc.do_connect("127.0.0.1 6502")

    # Tell the disassemble command to only disassemble one command at a time
    dc.do_disass("0 1")

    iNum = 0
    lastPc = -1
    curPc = 0
    while( (iNum < numSteps) or (numSteps == -1) ):

        dc.do_disass("")
        assemblyText = dc.getLastResult().strip()

        # Force the assembley text which is variable length, to fixed length
        asTextLen = len(assemblyText)
        padding = 45 - asTextLen
        assemblyText += " " * padding

        dc.do_step("")
        regsOut = dc.getLastResult()

        regsLines = regsOut.strip().split("\n")
        if (len(regsLines) != 4):
            traceOuput.write("TRACE FAILED\n")
            break


        # Since the length of the flags line varies, print it last
        rearranged = [ regsLines[0], regsLines[1], regsLines[3], regsLines[2]]
        traceOutput.write("TRACE> {}{}\n".format(assemblyText, "    ".join(rearranged)))

        # Determine what the current PC is
        m = re.search("PC=([0-9a-f]*)", regsLines[1])
        if ( (m == None) or (m.groups(1) == None) ):
            print("Failed to determine PC")

            if (numSteps == -1):
                print("Exitting trace utility, we cant determine PC")
                break
        else:
            print("PC = {}".format(m.group(1)))
            curPc = int(m.group(1), 16)

            if (curPc == lastPc):
                print("Emulator appears to be stuck on instruction / unable to step, exitting")
                break

            # Remember this PC for the next loop now
            lastPc = curPc
        iNum += 1

    # Any memory dumps requested?
    for curDump in dumpList:
        traceOutput.write("\n")

        dc.do_md(hex(curDump)[2:])
        traceOutput.write(dc.getLastResult())

    # After telling the emulator to shutdown, wait for the process to exit
    dc.do_shutdown("")
    p.wait()

    print("Child process finished, closing files")

    emulatorStdOut.close()
    emulatorStdErr.close()
    traceOutput.close()


if __name__ == "__main__":
    main(sys.argv)
