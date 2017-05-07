    cpu 6502
          cout = $fded ; display a character
             * = $300  ; assemble at $300
               code
   BRK
   ORA ($34,X)
   ORA $45
   ASL $ab
   ORA $1234
   PHP
   ORA $dead
   ASL $beef

   BPL *+$02
   ORA ($12), Y
   ORA $99, X
   ASL $EE, X
   CLC
   ORA $face, Y
   ORA $a555, X
   ASL $d1c5, X



