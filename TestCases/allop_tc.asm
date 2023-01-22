
;Test all opcodes both w decimal and hexadecimal operands (except .ORIG) -- not extensive tho
.ORIG 0x1000

FOO ADD R7, R0, R1
    ADD R7, R0, #16 ;
    AND R7, R0, R1  ;
    AND R7, R0, #16 ;
    JMP R4          ;0xC100
    JSR FOO         ;
    JSRR R3         ;0x40C0
    LDB R2, R4, x0  ;0x2500
    LDW 
    LEA
    NOT
    LSHF
    RSHFL
    RSHFA
    STB
    STW
    XOR

    HALT 
    RET
    RTI
    TRAP
    NOP

    BRn
    BRp
    BRnp
    BRz
    BRnz
    BRzp
    BRnzp
    BR

    .FILL

.END

;-----OUTPUT-----
;0x1000
