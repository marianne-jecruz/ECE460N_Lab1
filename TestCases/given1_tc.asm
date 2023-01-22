	
	.ORIG #4096
A	LEA R1, Y
	LDW R1, R1, #0
	LDW R1, R1, #0
	ADD R1, R1, R1
	ADD R1, R1, x-10	;x-10 is the negative of x10
	BRN A
	HALT
Y	.FILL #263
	.FILL #13
	.FILL #6
	.END


 ;NOTE: every run of readAndParse 
 ;		starts at a different line 

;-----OUTPUT-----
;	0x1000
;	0xE206
;	0x6240
;	0x6240
;	0x1241
;	0x1270
;	0x09FA
;	0xF025
;	0x0107
;	0x000D
;	0x0006
