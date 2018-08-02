#!/usr/bin/env python

import sys
import struct

def main(args):
   if (len(args) != 3):
      print("Usage: {} hexRomInput.rom output.bin".format(args[0]))
      return

   inputFile = open(args[1], 'r')
   outputFile = open(args[2], 'w+')
   ignoreFile = open("ignore.txt", 'w+')

   for eachLine in inputFile:
      eachLine = eachLine.strip()
      if eachLine == "":
         print("Ignoring a blank line")
         continue

      if eachLine.startswith("//"):
         print("Ignoring: {}".format(eachLine))
         ignoreFile.write(eachLine + "\n")

      else:
         byteVal = int(eachLine, 16)
         print("Parsing {} = {}".format(eachLine, hex(byteVal)))
         outputFile.write(struct.pack("B", byteVal) )
  
   inputFile.close()
   outputFile.close()





if __name__ == "__main__":
   main(sys.argv)
