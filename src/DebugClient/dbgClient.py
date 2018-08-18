#!/usr/bin/env python

# Emulator 6502 debugger client.  Uses cmd2 to provide an interactive shell

import struct
import sys
import cmd2
import socket
import struct

def prettyhex(number, numBits):
    hexNumber = hex(number)[2:]

    numChars = numBits / 4
    numCharsToAdd = numChars - len(hexNumber) 

    return "0" * numCharsToAdd + hexNumber

def hexDump(dataAddr, data, numBytes):
    dumpLineStart = dataAddr & 0xfff0
    dumpLineEnd   = (dataAddr + numBytes - 1) | 0xf

    dataEnd = dataAddr + numBytes

    numDumpLines = (dumpLineEnd + 1 - dumpLineStart) / 16
    retVal = ""
    asciiLine = ""
    print("Memory dump debug: dumpStart = {}, dumpEnd = {}".format(prettyhex(dumpLineStart, 16), prettyhex(dumpLineEnd, 16)))
    for curAddr in range(dumpLineStart, dumpLineEnd + 1):
        # Print out the address
        if ( (curAddr & 0xfff0) == curAddr ):
            # This is the beginning of a line, print address
            retVal += prettyhex(curAddr, 16)
            retVal += "    "
            #print("Starting new line @ {}".format(prettyhex(curAddr, 16)))
            asciiLine = "    |"

        # Print out the data bytes
        if ( (curAddr >= dataAddr) and (curAddr < dataEnd) ):
            # print("Going to add char at address {}, index {} in data".format(prettyhex(curAddr, 16), curAddr - dataAddr))

            ordVal = ord(data[curAddr - dataAddr])
            retVal += prettyhex(ordVal, 8)

            # add the character to teh ascii buf at the end of the line
            if ( (ordVal >= 0x20) and (ordVal <= 0x7e) ):
                asciiLine += chr(ordVal)
            else:
                asciiLine += "."
        else:
            print("Address {} is a blank in the md".format(prettyhex(curAddr, 16)))
            retVal += "  "
            asciiLine += " "

        retVal += " "

        # Print a space after the 8th byte for easy reading
        if ( (curAddr & 0xf) == 0x7):
            retVal += " "

        #Print the line ending on the last character of the line
        if ( (curAddr & 0xf) == 0xf):
            asciiLine += "|"

            retVal += asciiLine

            retVal += "\n"

    return retVal




def main(argv):
    # Defaults
    ipAddress = "127.0.0.1"
    portNum = 6502

    for singleArg in argv:
        if (singleArg.startswith("--ip=")):
            ipAddress = singleArg[5:]
            argv.remove(singleArg)
            continue
        if (singleArg.startswith("--port=")):
            portNum = int(singleArg[7:])
            argv.remove(singleArg)
            continue
        if ( (singleArg == "--help") or (singleArg == "-h") ):
            print("Usage:  {} --ip=127.0.0.1 --port=6502".format(argv[0]))
            return

    print("Attempting to connect to debug server {}:{}".format(ipAddress, portNum))

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ipAddress, portNum))
    except IOError as e:
        print("Failed to connect to the debugger server: {}".format(e.message))
        #return

    # Pop off the first 2 args, or cmd2 will try to parse them

    dc = DbgClient(s)
    dc.cmdloop()


class DbgClient(cmd2.Cmd):

    def __init__(self, s = None):
        cmd2.Cmd.__init__(self)

        self.intro = "Debugger Client for 6502 Emulator"
        self.prompt = "6502> "

        self.s = s

        self.lastResult = ""

    def getLastResult(self):
        return self.lastResult

    def sendHeader(self, opCode, messageLength):
        header = struct.pack("!HH", messageLength, opCode)
        self.s.send(header)

    def receiveMessage(self):
        frameHeader = self.s.recv(2)

        frameSize = struct.unpack("!H", frameHeader)[0]
        # print("Frame header, indicates message of {} bytes to be received".format(frameSize))

        if (frameSize > 0):
            retData = self.s.recv(frameSize)
            # print("Received {} frame bytes".format(len(retData)))
            return retData
        else:
            print("No data in frame to receive, return empty string")
            return ""

    def do_connect(self, argList):
        """
        Connects to a 6502 emulator

        Usage:  connect ip port
        """
        args = argList.split()

        if (len(args) != 2):
            print("Usage: connect ip port")
            return

        ipAddress = args[0]
        portNum = int(args[1])

        print("Attempting to connect to debug server {}:{}".format(ipAddress, portNum))

        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.connect((ipAddress, portNum))
            print("Connected")
            self.lastResult = "Connected"
        except IOError as e:
            print("Failed to connect to the debugger server: {}".format(e.message))
            self.lastResult = "Failed to Connect"
            return

    def do_about(self, args):
        """
        Get version information about debugger client and emulator
        """
        self.sendHeader(1, 0)

        emuVersion = self.receiveMessage()

        self.lastResult = "Emulator: {}\n".format(emuVersion)
        self.lastResult += "Client: Python 6502 Debug Client v0.0"
        print(self.lastResult)

    def do_shutdown(self, args):
        """
        Tells the emulator to exit
        """
        self.sendHeader(2, 0)

    def do_disass(self, args):
        """
        Disassembles code

        disass [address] [numInstructinos]

        Address is always interpreted as hexadecimal
        """

        argList = args.split()
        
        if (len(argList) == 0):
            flags = 0     # no address, no num instructions
            address = 0
            num = 0
        elif(len(argList) == 1):
            flags = 1
            address = int(argList[0], 16)
            num = 0
        else:
            flags = 3
            address = int(argList[0], 16)
            num = int(argList[1])

        print("Sending disassemble command(flags = {}, address = {}, numInstructions = {}".format(flags, hex(address), num))

        self.sendHeader(3, 5)
        msgData = struct.pack("!BHH", flags, address, num)
        self.s.send(msgData)

        self.lastResult = self.receiveMessage()
        print("Listing:\n{}".format(self.lastResult))


    def do_regs(self, args):
        """
        Dumps the registers
        """

        self.sendHeader(4,0)
        self.receiveRegisterData()

    def do_step(self, argList):
        """
        Step the debugger some finite number of instructions.

        Default is 1 instruction, or you can provide number of instructions to step
        """
        args = argList.split()
        if (len(args) == 0):
           stepCount = 1
        else:
           stepCount = int(args[0], 16)

        print("Stepping {} (0x{}) steps".format(stepCount, prettyhex(stepCount, 16)))

        self.sendHeader(5,2)

        msgBody = struct.pack("!H", stepCount)
        self.s.send(msgBody)

        self.receiveRegisterData()

    def do_halt(self, args):
        """
        Halt the emulator so we can debug
        """
        self.sendHeader(6,0)
        self.receiveRegisterData()

    def do_continue(self, args):
        """
        Continue emulator execution
        """

        self.sendHeader(7,0)
        self.receiveRegisterData()
   
    def receiveRegisterData(self):
        rspData = self.receiveMessage()

        # Going to return X, Y, Accum, StackPointer, PC, statusReg, Padding, NumClocksHigh, NumClocksLow

        (x, y, accum, sp, pc, status, padding, numClkHigh, numClkLow) = struct.unpack("!BBBBHBBLL", rspData)

        self.lastResult  = " X={}   Y={}    A={}\n".format(prettyhex(x, 8), prettyhex(y, 8), prettyhex(accum, 8))
        self.lastResult += "SP={}  PC={}\n".format(prettyhex(sp, 8), prettyhex(pc, 16))

        statusFlags = []
        if (status & 0x01):
            statusFlags.append("FLG_CARY")
        if (status & 0x02):
            statusFlags.append("FLG_ZERO")
        if (status & 0x04):
            statusFlags.append("FLG_INTD")
        if (status & 0x08):
            statusFlags.append("FLG_DECI")
        if (status & 0x10):
            statsuFlags.append("FLG_BKPT")
        if (status & 0x40):
            statusFlags.append("FLG_OVFL")
        if (status & 0x80):
            statusFlags.append("FLG_NEG")

        self.lastResult += "SR={} = {}\n".format(prettyhex(status, 8), " | ".join(statusFlags))

        self.lastResult += "Num Clocks = {}{}".format(prettyhex(numClkHigh, 32), prettyhex(numClkLow, 32))

        print(self.lastResult)

    def do_md(self, argstr):
        """
        Memory dump.

        md address [numbytes]

        Defaults to dumping 0x100 bytes
        """

        args = argstr.split()
        
        if (len(args) < 1):
            self.lastResult = "Memory Dump failed, no address specified"
            print(self.lastResult)
            return

        address = int(args[0], 16)
        numBytes = 0x100
        if (len(args) >= 2):
            numBytes = int(args[1], 16)

        print("Requesting a memory dump of {} bytes at address {}".format(prettyhex(numBytes, 16), prettyhex(address, 16)))

        self.sendHeader(8,4)

        msgData = struct.pack("!HH", address, numBytes)
        self.s.send(msgData)

        rsp = self.receiveMessage()

        (addrRx, bytesRx) = struct.unpack("!HH", rsp[0:4])
        print("Received {} bytes from address {}".format(prettyhex(bytesRx, 16), prettyhex(addrRx, 16)))

        dataBuf = rsp[4:]
        if (len(dataBuf) < bytesRx):
            print("Didn't receive the number of bytes indicated in memory dump")
            print("Raw Dump of Msg:")
            print(rsp)
            return

        self.lastResult = hexDump(addrRx, dataBuf, bytesRx)
        print(self.lastResult)

if (__name__ == "__main__"):
    main(sys.argv)
