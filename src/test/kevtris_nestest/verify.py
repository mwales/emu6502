#!/usr/bin/env python

import sys
import subprocess

def main(args):
    try:
        po = subprocess.check_output(["./emu6502", "-c", "nestest.json", "-n", "8900"])
    except:
        print("We encountered some kind of error during the trace execution, oh well")

    verifyFiles()


'''
Nestest output:
F790  AD 47 06  LDA $0647 = 14                  A:57 X:FF Y:14 P:27 SP:FB CYC:103

Debugger based verification:
TRACE> c739     4c 40 c7     JMP $c740                  X=00   Y=00    A=00    SP=fb  PC=c740    Num Clocks = 0000000000000024    SR=26 = FLG_ZERO | FLG_INTD

trace.txt from emu6502 (which looks like the following, but it has tabs
c735    ea           NOP                        A=00,  X=00,  Y=00,  SP=fb,  SR=27,  CLK=000000000000001b

00000000  63 30 30 30 09 34 63 20  66 35 20 63 35 20 20 20  |c000.4c f5 c5   |
00000010  20 20 4a 4d 50 20 24 63  35 66 35 20 20 20 20 20  |  JMP $c5f5     |
00000020  20 20 20 20 20 20 20 20  20 20 20 20 20 41 3d 30  |             A=0|
00000030  30 2c 20 20 58 3d 30 30  2c 20 20 59 3d 30 30 2c  |0,  X=00,  Y=00,|
00000040  20 20 53 50 3d 66 64 2c  20 20 53 52 3d 32 34 2c  |  SP=fd,  SR=24,|
00000050  20 20 43 4c 4b 3d 30 30  30 30 30 30 30 30 30 30  |  CLK=0000000000|
00000060  30 30 30 30 30 30 0a 63  35 66 35 09 61 32 20 30  |000000.c5f5.a2 0|

'''

def verifyFiles():
    verification = open("test_src_files/nestest_good_output.log", "r")
    underTest = open("trace.txt")

    verificationText = verification.read().split("\n")
    underTestText = underTest.read().split("\n")

    # Trip off the useless text in trace.txt
    underTestText = underTestText[1:-2]

    i = 0
    clockCyles = -1
    while(True):
        print("Checking line {}".format(i))

        ut = underTestText[i]
        vt = verificationText[i]

        #print("ut = " + ut)
        #print("vt = " + vt)

        if ((len(ut) < 96) or (len(vt) < 73)):
            print("End of process log @ line {}, no diffs found!".format(i))
            print("Length of under test = {}, length of verification = {}".format(len(ut), len(vt)))
            return

        addrUut = int(ut[0:4], 16)
        addrVerify = int(vt[0:4], 16)

        # Check instruction address
        if (addrUut != addrVerify):
            print("Address of execution differs: {} vs {}".format(hex(addrUut), hex(addrVerify)))
            break
        else:
            print("Address of execution same: {} vs {}".format(hex(addrUut), hex(addrVerify)))

        # Check X and Y registers
        print("x ut=" + ut[0x36:0x38] + "  vt=" + vt[55:57])
        xUut = int(ut[0x36:0x38], 16)
        xVerify = int(vt[55:57], 16)
        if (xUut != xVerify):
            print("X register value differs: {} vs {}".format(hex(xUut), hex(xVerify)))
            break
        else:
            print("X register value same: {} vs {}".format(hex(xUut), hex(xVerify)))

        yUut = int(ut[0x3d:0x3f], 16)
        yVerify = int(vt[60:62], 16)
        if (yUut != yVerify):
            print("Y register value differs: {} vs {}".format(hex(yUut), hex(yVerify)))
            break
        else:
            print("Y register value same: {} vs {}".format(hex(yUut), hex(yVerify)))

        # Check accumulator
        accumUut = int(ut[0x2f:0x31], 16)
        accumVerify = int(vt[50:52], 16)
        if (accumUut != accumVerify):
            print("Accumulator value differs: {} vs {}".format(hex(accumUut), hex(accumVerify)))
            break
        else:
            print("Accumulator value same: {} vs {}".format(hex(accumUut), hex(accumVerify)))

        # Check the stack pointer
        spUut = int(ut[0x45:0x47], 16)
        spVerify = int(vt[71:73], 16)
        if (spUut != spVerify):
            print("Stack pointer value differs: {} vs {}".format(hex(spUut), hex(spVerify)))
            break
        else:
            print("Stack pointer value same: {} vs {}".format(hex(spUut), hex(spVerify)))

        # Check the status register
        srUut = int(ut[0x4d:0x4f], 16)
        srVerify = int(vt[65:67], 16)
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







