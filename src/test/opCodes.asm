    cpu 6502
          cout = $fded ; display a character
             * = $300  ; assemble at $300
               code
   BRK
   ORA ($34,X)
   ORA $45
   ORA $1234
   PHP
   ASL $ab

