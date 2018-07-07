#!/usr/bin/env python

# I use this script to convert some text documents I copied from a couple
# of websites into a giant macro that I can incorporate into my source
# code.

allCodeFile = open("OpCodeDoc.txt", 'r')
flagsFile = open("OpCodeBitsSorted.txt", 'r')
outputFile = open("OpCodeOut.txt", 'w')

# Dictionary of stuff
# OpCode
#   Mnemonic
#   AddressMode
#   InstructionLen
#   Delay
#   ExtraDelay
#   FlagBits
MainDict = {}

OpCodeReached = False
for eachLine in allCodeFile:
   if OpCodeReached:
      if eachLine.find("#END_OP_CODES") != -1:
         print("Found the end of the opcodes")
	 OpCodeReached = False
	 continue

      CurInst = {}

      print("Parsing opcode line {}".format(eachLine))

      opCode = int(eachLine[3:5], 16)
      instructionLen = int(eachLine[19:20])
      delayCount = int(eachLine[21:22])

      # The mnemonic follows the op code, but length is not standard
      endOfMnemonic = eachLine.find(" ", 6)
      if (endOfMnemonic == -1):
         print("Error finding the mnemonic")
	 continue

      mnemonicText = eachLine[6:endOfMnemonic]

      # Throw away the leading astrik on mnemonic if it haves one
      if (mnemonicText[0] == '*'):
         mnemonicText = mnemonicText[1:]

      addressingModePos = endOfMnemonic + 1
      endOfAddressingMode = eachLine.find(" ", endOfMnemonic + 1)
      addressingModeText = eachLine[addressingModePos:endOfAddressingMode]

      CurInst['Delay'] = delayCount
      CurInst['InstructionLen'] = instructionLen
      CurInst['Mnemonic'] = mnemonicText
      CurInst['AddrMode'] = addressingModeText

      print("opCode {} is {}".format(hex(opCode), CurInst))
      
      # Add to the main dictionary that will be used to generate code
      MainDict[opCode] = CurInst

   else:
      if eachLine.find("#BEGIN_OP_CODES") != -1:
         print("Found the start of opcodes")
	 OpCodeReached = True
      else:
         print("Searching for opcode start, not {}".format(eachLine))


# Now we are going to read the file that has the best information about the
# flags are set by each opcode
OpCodeReached = False
for eachLine in flagsFile:
   CurInst = {}
   if OpCodeReached:
      if eachLine.find("#END_OP_CODES") != -1:
         print("Found the end of the opcodes in the flags file")
	 OpCodeReached = False
	 continue

      # Now we process the line and grab all the flags
      print("Now we will process {} for flags".format(eachLine))

      opCode = int(eachLine[0:2],16)

      extraDelay = eachLine[9:10]

      flags = eachLine[16:24]

      CurInst = MainDict[opCode]

      if (extraDelay == 'a'):
         CurInst['ExtraDelay'] = True
      else:
         CurInst['ExtraDelay'] = False

      CurInst['FlagBits'] = flags

      print("Now: {}".format(CurInst))
 
   else:
      if eachLine.find("#BEGIN_OP_CODES") != -1:
         print("Found the start of opcodes in the flags file")
	 OpCodeReached = True
      else:
         print("Searching for opcode start, not {}".format(eachLine))

AddrModeToEnumVal = { "imm": "IMMEDIATE  ",
                      "rel": "RELATIVE   ",
                      "izx": "INDIRECT_X ",
                      "izy": "INDIRECT_Y ",
                      "abs": "ABSOLUTE   ",
                      "abx": "ABS_ZP_X   ",
                      "aby": "ABS_ZP_Y   ",
                      "zp":  "ZERO_PAGE  ",
                      "zpx": "ZERO_PAGE_X",
                      "zpy": "ZERO_PAGE_Y",
                      "imp": "IMPLIED    ",
		      "ind": "INDIRECT   "}

FlagEnum = { 'N': "FLG_NEG ",
             'V': "FLG_OVFL",
	     'B': "FLG_BRK ",
	     '0': "FLG_DECI",
	     '1': "FLG_INTD",
	     'Z': "FLG_ZERO",
	     'C': "FLG_CARY",
	     "........": "FLG_NONE"}
      
# Now we need to output everything to the output file
for opCode in MainDict.keys():
   CurInst = MainDict[opCode]

   print("OUT: {} = {}".format(hex(opCode), CurInst))

   # Macro, OpCode, InstructionLen
   macroText = "   CPU_OP({}, {}, ".format(hex(opCode), CurInst['InstructionLen'])
   
   # Mnemonic String, Mnemonic Instruction
   macroText += "{}, {}, ".format(CurInst['Mnemonic'], CurInst['Mnemonic'].lower())
   
   # Addressing mode
   addrModeAbbreviation = CurInst['AddrMode']
   addrModeText = AddrModeToEnumVal[addrModeAbbreviation]
   macroText += addrModeText

   macroText += ", {}, ".format(CurInst['Delay'])

   if "ExtraDelay" in CurInst:
      if CurInst['ExtraDelay']:
         macroText += "true,  "
      else:
         macroText += "false, "
   else:
      macroText += "false, "

   numFlags = 0
   if "FlagBits" in CurInst:
      curFlagBits = CurInst['FlagBits']
      for possibleFlags in FlagEnum.keys():
         if (curFlagBits.find(possibleFlags) != -1):
	    # Need to print out the current flag!
            if numFlags == 0:
	       macroText += FlagEnum[possibleFlags]
	    else:
	       macroText += " | " + FlagEnum[possibleFlags]
	    numFlags += 1 
   else:
      macroText += "FLG_NONE"
   
   macroText += ")"

   print(macroText)
   outputFile.write(macroText + "\n")

