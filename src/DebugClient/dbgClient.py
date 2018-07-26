#!/usr/bin/env python

# Emulator 6502 debugger client.  Uses cmd2 to provide an interactive shell

import struct
import sys
import cmd2
import socket
import struct

def main(argv):
    if (len(argv) != 3):
        print("Usage:  {} ip port".format(argv[0]))
        return

    ipAddress = argv[1]
    portNum = int(argv[2])

    print("Attempting to connect to debug server {}:{}".format(ipAddress, portNum))

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ipAddress, portNum))
    except IOError as e:
        print("Failed to connect to the debugger server: {}".format(e.message))
        #return

    # Pop off the first 2 args, or cmd2 will try to parse them
    sys.argv.pop(0)
    sys.argv.pop(0)

    dc = DbgClient(s)
    dc.cmdloop()


class DbgClient(cmd2.Cmd):

    def __init__(self, s):
        cmd2.Cmd.__init__(self)

        self.intro = "Debugger Client for 6502 Emulator"
        self.prompt = "6502> "

        self.s = s

    def sendHeader(self, opCode, messageLength):
        header = struct.pack("!HH", messageLength, opCode)
        self.s.send(header)

    def receiveMessage(self):
        frameHeader = self.s.recv(2)

        frameSize = struct.unpack("!H", frameHeader)[0]
        print("Frame header, indicates message of {} bytes to be received".format(frameSize))

        if (frameSize > 0):
            retData = self.s.recv(frameSize)
            print("Received {} frame bytes".format(len(retData)))
            return retData
        else:
            print("No data in frame to receive, return empty string")
            return ""

    def do_stuff(self, args):
        """
        This is the awesome doucmentation for the stuff command!
        """
        print("Hey, look at me: {}".format(args))

    def do_about(self, args):
        """
        Get version information about debugger client and emulator
        """
        self.sendHeader(1, 0)

        emuVersion = self.receiveMessage()
        print("Emulator: {}".format(emuVersion))

    def do_shutdown(self, args):
        """
        Tells the emulator to exit
        """
        self.sendHeader(2, 0)




if (__name__ == "__main__"):
    main(sys.argv)