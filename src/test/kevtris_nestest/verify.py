#!/usr/bin/env python

import sys
import subprocess

def main(args):
    try:
        po = subprocess.check_output(["./traceUtility.py", "--config", "nestest.json", "-t=8900"])
    except:
        print("We encountered some kind of error during the trace execution, oh well")

    verifyFiles()


def verifyFiles():
    verification = open("test_src_files/nestest_good_output.log", "r")
    underTest = open("trace_output.txt")

    verificationText = verification.read().split("\n")
    underTestText = underTest.read().split("\n")

    i = 0
    clockCyles = -1
    while(True):
        print("Checking line {}".format(i))

        ut = underTestText[i]
        vt = verificationText[i]

        vtState = verificationText[i+1]

        if ((len(ut) < 131) or (len(vtState) < 73)):
            print("End of process log @ line {}, no diffs found!".format(i))
            print("Length of under test = {}, length of verification = {}".format(len(ut), len(vtState)))
            return

        addrUut = int(ut[7:11], 16)
        addrVerify = int(vt[0:4], 16)

        # Check instruction address
        if (addrUut != addrVerify):
            print("Address of execution differs: {} vs {}".format(hex(addrUut), hex(addrVerify)))
            break
        else:
            print("Address of execution same: {} vs {}".format(hex(addrUut), hex(addrVerify)))

        # Check X and Y registers
        xUut = int(ut[54:56], 16)
        xVerify = int(vtState[55:57], 16)
        if (xUut != xVerify):
            print("X register value differs: {} vs {}".format(hex(xUut), hex(xVerify)))
            break
        else:
            print("X register value same: {} vs {}".format(hex(xUut), hex(xVerify)))

        yUut = int(ut[61:63], 16)
        yVerify = int(vtState[60:62], 16)
        if (yUut != yVerify):
            print("Y register value differs: {} vs {}".format(hex(yUut), hex(yVerify)))
            break
        else:
            print("Y register value same: {} vs {}".format(hex(yUut), hex(yVerify)))

        # Check accumulator
        accumUut = int(ut[69:71], 16)
        accumVerify = int(vtState[50:52], 16)
        if (accumUut != accumVerify):
            print("Accumulator value differs: {} vs {}".format(hex(accumUut), hex(accumVerify)))
            break
        else:
            print("Accumulator value same: {} vs {}".format(hex(accumUut), hex(accumVerify)))

        # Check the stack pointer
        spUut = int(ut[78:80], 16)
        spVerify = int(vtState[71:73], 16)
        if (spUut != spVerify):
            print("Stack pointer value differs: {} vs {}".format(hex(spUut), hex(spVerify)))
            break
        else:
            print("Stack pointer value same: {} vs {}".format(hex(spUut), hex(spVerify)))

        # Check the status register
        srUut = int(ut[129:131], 16)
        srVerify = int(vtState[65:67], 16)
        if (srUut != srVerify):
            print("Status Register value differs: {} vs {}".format(hex(srUut), hex(srVerify)))
            break
        else:
            print("Status Register value same: {} vs {}".format(hex(srUut), hex(srVerify)))

        print("All OK @ {} (unable to verify clock cycles".format(hex(addrUut)))

        # Check the clock cycles if possible
        #clockUut = int(ut[106:122], 16)
        #ppuVerify = int(vt[78:80], 16)
        #clockVerify = ppuVerify / 3
        #
        #if (previousClock == -1):
        #else:
        #    if (clockUut == previousClock):
        #        print("All OK @ {} (verified clock)".format(hex(addrUut)))
        #    else:



        i += 1

    startLine = i - 4
    endLine = i + 4
    if (startLine < 0):
        startLine = 0

    for lineNum in range(startLine, startLine + 8):
        if (lineNum == i):
            print("Diff Line:")
        
        print(verificationText[lineNum])
        print(underTestText[lineNum])






if __name__ == "__main__":
    main(sys.argv)







