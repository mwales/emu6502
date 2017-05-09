    cpu 6502
          cout = $fded ; display a character
             * = $300  ; assemble at $300
               code

   ; All the load opcodes
   LDA #$44
   LDA $44
   LDA $44,X
   LDA $4400
   LDA $4400,X
   LDA $4400,Y
   LDA ($44,X)
   LDA ($44),Y

   LDX #$44
   LDX $44
   LDX $44,Y
   LDX $4400
   LDX $4400,Y

   LDY #$44
   LDY $44
   LDY $44,X
   LDY $4400
   LDY $4400,X

   ; All the store opcodes
   STA $44
   STA $44,X
   STA $4400
   STA $4400,X
   STA $4400,Y
   STA ($44,X)
   STA ($44),Y

   STX $44
   STX $44,Y
   STX $4400

   STY $44
   STY $44,X
   STY $4400




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



