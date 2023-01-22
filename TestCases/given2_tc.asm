;This program counts from 10 to 0
	.ORIG x3000   	
	LEA R0, TEN		;This instruction will be loaded into memory location x3000
	LDW R1, R0, #0
START	ADD R1, R1, #-1
	BRZ DONE
	BR START
				;blank line
DONE	TRAP x25		;The last executable instruction
TEN	.FILL x000A		;This is 10 in 2's comp, hexadecimal
	.END			;The pseudo-op, delimiting the source program

;-----OUTPUT-----
;	0x3000
;	0xE005
;	0x6200
;	0x127F
;	0x0401
;	0x0FFD
;	0xF025
;	0x000A
