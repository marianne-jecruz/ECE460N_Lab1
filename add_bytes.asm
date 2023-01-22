
.ORIG x3000

;R0 = result
;R2 = num1
;R3 = num2
;R4 = x3100
;R5 = x0

        LEA R0, ADRZERO
        LDW R5, R0, x0
        
        LEA R0, ADRNUM1 ;address associated w/ label ADRNUM1 loaded into R0
        LDW R4, R0, x0 ;contents of addr loc R0 loaded into R4 = x3100
        
        STW R5, R4, x3 ;mem[x3103] = 0 --> no overflow until changed otherwise

        LDB R2, R4, x0 ;num1 loaded into R2
        BRn NEGBR
        
        ;num1 is positive
        LDB R3, R4, x1 ;num2 loaded into R3
        BRn MIXBR
        
        ;num1 & num2 are positive
        ADD R0, R2, R3  ;R0 = result
        BRp STRRES      ;if positive, no overflow
        ADD R5, R5, x1  ;R5 = 1
        STW R5, R4, x3  ;mem[x3103] = 1
        BR STTRES

        ;num1 negative
NEGBR   LDB R3, R4, x1 ;num2 loaded into R3
        BRzp MIXBR
        
        ;num1 & num2 are negative
        ADD R0, R2, R3  ;R0 = result
        BRn STRRES
        ADD R5, R5, x1  ;R5 = 1
        STW R5, R4, x3  ;mem[x3103] = 1
        BR STTRES

        ;num1 and num2 = negpos or posneg
MIXBR   ADD R0, R2, R3 ;R0 = result

STRRES  STB R0, R4, x2


ADRNUM1 .FILL x3100
ADRZERO .FILL x0

.END