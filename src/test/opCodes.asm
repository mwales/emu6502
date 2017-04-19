    cpu 6502
          cout = $fded ; display a character
             * = $300  ; assemble at $300
               code
	brk
;	ora ($1234,X)
	php
	asl a

