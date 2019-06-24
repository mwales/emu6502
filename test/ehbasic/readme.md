# EhBASIC

The ehbasic.bin file is from a [code golfing post at stackoverflow[(https://codegolf.stackexchange.com/questions/12844/emulate-a-mos-6502-cpu).
The post author stated that he compiled the EhBASIC into 6502 binary, and
made available for [download](http://rubbermallet.org/ehbasic.zip).

I also found a [github site with the source](https://github.com/jefftranter/6502/tree/master/asm/ehbasic)
and some licensing information about EhBASIC.

## My modifications

The code golfing post indicates that the 6502 processor can write to the UART
by writing a byte to address 0xF001, and reads a byte from the UART by reading
from address 0xF004.

The ehbasic.bin file is supposed to be loaded at 0xC000 and fills up the rest
of the memory all the way up to 0xffff.  Since my emulator doesn't like the
memory regions to overlap (the UART address is in conflict), I had to split the
ROM into 2 pieces.  The addresses contained in the ROM names explain where to
load them.

The configuration for this rom is ehbasic.json

# License Info

Enhanced BASIC is a BASIC interpreter for the 6502 family microprocessors. It
 is constructed to be quick and powerful and easily ported between 6502 systems.
 It requires few resources to run and includes instructions to facilitate easy
 low level handling of hardware devices. It also retains most of the powerful
 high level instructions from similar BASICs.

 EhBASIC is free but not copyright free. For non commercial use there is only one
 restriction, any derivative work should include, in any binary image distributed,
 the string "Derived from EhBASIC" and in any distribution that includes human
 readable files a file that includes the above string in a human readable form
 e.g. not as a comment in an HTML file.

 For commercial use please contact me,  Lee Davison, at leeedavison@googlemail.com
 for conditions.

 For more information on EhBASIC, other versions of EhBASIC and other projects
 please visit my site at ..

	 http://members.multimania.co.uk/leeedavison/index.html


 P.S. c't magazin, henceforth refered to as "those thieving german bastards", are
 prohibited from using this or any version of EhBASIC for any of their projects
 or products. The excuse "we don't charge people for it" doesn't wash, it adds
 value to your product so you owe me.



