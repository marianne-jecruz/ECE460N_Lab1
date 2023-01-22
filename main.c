#include <stdio.h> /* standard input/output library */
#include <stdint.h>
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */


#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
enum {DONE, OK, EMPTY_LINE}; //Enums for readAndParse 

/*------------Global Variables------------*/
FILE* in_File = NULL;
FILE* out_File = NULL; 
char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
    *lArg2, *lArg3, *lArg4;
int lRet;
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];	/*Question for the reader: Why do we need to add 1? : Need a null terminator*/
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS]; //array of length 255 made up of addresses + strings of labels
char* opcodeDictionary[] = {"and", "add", "jmp", "jsr", "jsrr", "ldb", 
                            "ldw", "lea", "not", "ret", "rti", "lshf",
                            "rshfl", "rshfa", "stb", "stw", "trap","xor",
                            "halt", "br", "brn", "brp", "brnp", "brz", "brnz", 
                            "brzp", "brnzp", "nop", "\0"};
char* regDictionary[] = {"r0","r1", "r2", "r3", "r4", "r5", "r6", "r7", "\0"};

int int_bin_array[12]; //bits 11 to 0
char* arg_bin_array;
int hex2, hex1, hex0;

/*------------Function Protos------------*/
int readAndParse( FILE *pInfile, char * pLine, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4 );
int toNum( char * pStr );
int isOpcode(char * word);
int appendSymbolTable(char *label, int cAddr);
int opcodeArbiter(char* opcode_str);
int regDecode(char* reg_str, int num_arr_flag, int start_bit);
int pow(int e, int t);
void arrToHex();


void main() {
    /* open the source file */
     in_File = fopen("tc1_given.asm", "r");
     out_File = fopen("tc1_out.obj", "w");
		 
     if (!in_File) {
       printf("Error: Cannot open file %s\n", "tc1_given.asm");
       exit(4);
		 }
     if (!out_File) {
       printf("Error: Cannot open file %s\n", "tc1_out.obj");
       exit(4);
     }

     /* Do stuff with files */

    /*FILE * lInfile;
    lInfile = fopen( "data.in", "r" );*/ /* open the input file */

    do{
    lRet = readAndParse(in_File, lLine, &lLabel,
        &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
    if( lRet != DONE && lRet != EMPTY_LINE ) //CHECK CONDITION
    {
        opcodeArbiter(lOpcode);
        //appendSymbolTable(lLabel, 100);
        printf("label = %s \n", lLabel);
        printf("opcode = %s \n", lOpcode);
        printf("Arg1 = %s \n", lArg1);
        printf("Arg2 = %s \n", lArg2);
        printf("Arg3 = %s \n", lArg3);
        printf("Arg4 = %s \n", lArg4);
        printf("------------------------------\n");
    }
    } while( lRet != DONE );

     fclose(in_File);
     fclose(out_File);
}


/*------------Function Declarations------------*/
int readAndParse( FILE *pInfile, char * pLine, char ** pLabel, char
** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4 )
{
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
    return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
    pLine[i] = tolower( pLine[i] );
    
        /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' ) 
    {lPtr++;}

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
    return( EMPTY_LINE );

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ){ /* found a label */
    *pLabel = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }
        *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    
        *pArg1 = lPtr;
    
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

        *pArg2 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

        *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

        *pArg4 = lPtr;

    return( OK );
}

/*
Input: pointer to string
Output: -1 = Not a valid opcode
         1 = Valid opcode
- Valid Opcodes: ADD, AND, BR(all 8 variations), HALT, JMP, JSR, JSRR, LDB, LDW,
LEA, NOP, NOT, RET, LSHF, RSHFL, RSHFA, RTI, STB, STW, TRAP, XOR
    BR variations: 	BRn LABEL			BRz LABEL
	                BRp LABEL			BRnz LABEL
	                BRnp LABEL			BRzp LABEL
	                BR LABEL			BRnzp LABEL
*/
int isOpcode(char * word){
    int i;
    for(i = 0; (strcmp(opcodeDictionary[i],"\0")); i++){
        if (!(strcmp(opcodeDictionary[i],word))){return 1;}
    }
    return -1;
}

/*Note: Each time this is called, it fills a new element in symbol table array, so only
  call when there is for certain a label */
int appendSymbolTable(char *label, int cAddr){
    static int i = 0;
    int j;
    printf("%d", i);

    symbolTable[i].address = cAddr;

    for(j = 0; *(label + j) != '\0'; j++){
        symbolTable[i].label[j] = *(label + j);
    }
    symbolTable[i].label[j] = '\0';
    //printf("%s \n", &(symbolTable[i].label[0])); //test if the string I put in the symbolTable element returns correctly 
    i++;
}


/*------------OPCODE FUNCTIONS------------*/
/*LSHF = 1101 DR SR 00 amount4*/
void lshf_f(){
    int amount4;
    int_bin_array[5] = 0;
    int_bin_array[4] = 0;

    regDecode(lArg1, 1, 11);
    regDecode(lArg2, 1, 8);

    amount4 = toNum(lArg3);
    int_bin_array[3] = amount4/8;
    amount4 = amount4 % 8;
    int_bin_array[2] = amount4/4;
    amount4 = amount4 % 4;
    int_bin_array[1] = amount4/2;
    amount4 = amount4 % 2;
    int_bin_array[0] = amount4;

    arrToHex();
    fprintf(out_File, "xD%X%X%X\n", hex2, hex1, hex0);
    //printf("xD%X%X%X\n", hex2, hex1, hex0);
}

/*RSHFL = 1101 DR SR 01 amount4*/
void rshfl_f(){
    int amount4;
    int_bin_array[5] = 0;
    int_bin_array[4] = 1;

    regDecode(lArg1, 1, 11);
    regDecode(lArg2, 1, 8);

    amount4 = toNum(lArg3);
    int_bin_array[3] = amount4/8;
    amount4 = amount4 % 8;
    int_bin_array[2] = amount4/4;
    amount4 = amount4 % 4;
    int_bin_array[1] = amount4/2;
    amount4 = amount4 % 2;
    int_bin_array[0] = amount4;

    arrToHex();
    fprintf(out_File, "xD%X%X%X\n", hex2, hex1, hex0);
}

/*RSHFA = 1101 DR SR 11 amount4*/
void rshfa_f(){
    int amount4;
    int_bin_array[5] = 1;
    int_bin_array[4] = 1;

    regDecode(lArg1, 1, 11);
    regDecode(lArg2, 1, 8);

    amount4 = toNum(lArg3);
    int_bin_array[3] = amount4/8;
    amount4 = amount4 % 8;
    int_bin_array[2] = amount4/4;
    amount4 = amount4 % 4;
    int_bin_array[1] = amount4/2;
    amount4 = amount4 % 2;
    int_bin_array[0] = amount4;

    arrToHex();
    fprintf(out_File, "xD%X%X%X\n", hex2, hex1, hex0);
}

/*STB = 0011 SR BaseR boffset6*/
void stb_f(){
    int boffset6;
    regDecode(lArg1, 1, 11);
    regDecode(lArg2, 1, 8);

    boffset6 = toNum(lArg3);
    int_bin_array[5] = boffset6/32;
    boffset6 = boffset6 % 32;
    int_bin_array[4] = boffset6/16;
    boffset6 = boffset6 % 16;
    int_bin_array[3] = boffset6/8;
    boffset6 = boffset6 % 8;
    int_bin_array[2] = boffset6/4;
    boffset6 = boffset6 % 4;
    int_bin_array[1] = boffset6/2;
    boffset6 = boffset6 % 2;
    int_bin_array[0] = boffset6;

    arrToHex();
    fprintf(out_File, "x3%X%X%X\n", hex2, hex1, hex0);
}

/*STW = 0111 SR BaseR offset6*/
void stw_f(){
    int offset6;
    regDecode(lArg1, 1, 11);
    regDecode(lArg2, 1, 8);

    offset6 = toNum(lArg3);
    int_bin_array[5] = offset6/32;
    offset6 = offset6 % 32;
    int_bin_array[4] = offset6/16;
    offset6 = offset6 % 16;
    int_bin_array[3] = offset6/8;
    offset6 = offset6 % 8;
    int_bin_array[2] = offset6/4;
    offset6 = offset6 % 4;
    int_bin_array[1] = offset6/2;
    offset6 = offset6 % 2;
    int_bin_array[0] = offset6;

    arrToHex();
    fprintf(out_File, "x7%X%X%X\n", hex2, hex1, hex0);
}

/*XOR = 1001 DR SR1 0 00 SR2
  XOR = 1001 DR SR  1 imm5
  */
void xor_f(){
    int i;
    regDecode(lArg1, 1, 11);
    regDecode(lArg2, 1, 8);

    //determines if lArg3 is SR2 or imm5
    for(i=0; *(lArg3 + i) != '\0'; i++){
        if(*(lArg3 + i) == 'r'){
            int_bin_array[5] = 0;
            break;
        }
        else{int_bin_array[5] = 1;}
    }

    //bit5 = 0
    if(!int_bin_array[5]){
        int_bin_array[4] = 0;
        int_bin_array[3] = 0;
        regDecode(lArg3, 1, 2);
    }
    
    //bit5 = 1
    else{
        int imm5 = toNum(lArg3);
        int_bin_array[4] = imm5/16;
        imm5 = imm5 % 16;
        int_bin_array[3] = imm5/8;
        imm5 = imm5 % 8;
        int_bin_array[2] = imm5/4;
        imm5 = imm5 % 4;
        int_bin_array[1] = imm5/2;
        imm5 = imm5 % 2;
        int_bin_array[0] = imm5;
    }

    arrToHex();
    fprintf(out_File, "x9%X%X%X\n", hex2, hex1, hex0);
}

/*NOT = 1001 DR SR 111111*/
void not_f(){
    int i;
    for(i = 0; i < 6; i++){
        int_bin_array[i] = 1;
    }
    regDecode(lArg1, 1, 11);
    regDecode(lArg2, 1, 8);

    arrToHex();
    fprintf(out_File, "x9%X%X%X\n", hex2, hex1, hex0);
}

/*TRAP = 0000 trapvect8*/
void trap_f(){
    int decimal = 0;

    //check if opcode = HALT bc it sends HALT opcode to this function
    if( !strcmp(lOpcode, "halt")){
        fprintf(out_File, "xF025\n");
        //printf("xF025\n");
    }
    else{
        decimal = toNum(lArg1);
        int_bin_array[7] = decimal/128;
        decimal = decimal % 128;
        int_bin_array[6] = decimal/64;
        decimal = decimal % 64;
        int_bin_array[5] = decimal/32;
        decimal = decimal % 32;
        int_bin_array[4] = decimal/16;
        decimal = decimal % 16;
        int_bin_array[3] = decimal/8;
        decimal = decimal % 8;
        int_bin_array[2] = decimal/4;
        decimal = decimal % 4;
        int_bin_array[1] = decimal/2;
        decimal = decimal % 2;
        int_bin_array[0] = decimal;

        arrToHex();
        fprintf(out_File, "xF0%X%X\n", hex1, hex0);
        //printf("xF0%X%X\n", hex1, hex0);
    }
}

/*RTI = 1000 0's*/
void rti_f(){
    fprintf(out_File, "x8000\n");
}

void ret_f(){
   fprintf(out_File, "xC1C0\n");
}

void nop_f(){
    fprintf(out_File, "x0000\n");
}

int opcodeArbiter(char* opcode_str){
    if(!(strcmp(opcode_str, "add"))){return 1;} //strcmp = 0:when matches, = anything else:no match
    else if(!(strcmp(opcode_str, "and"))){return 1;}
    else if(!(strcmp(opcode_str, "halt"))){trap_f();}
    else if(!(strcmp(opcode_str, "jmp"))){return 1;}
    else if(!(strcmp(opcode_str, "jsr"))){return 1;}
    else if(!(strcmp(opcode_str, "jsrr"))){return 1;}
    else if(!(strcmp(opcode_str, "ldb"))){return 1;}
    else if(!(strcmp(opcode_str, "ldw"))){return 1;}
    else if(!(strcmp(opcode_str, "lea"))){return 1;}
    else if(!(strcmp(opcode_str, "nop"))){nop_f();}
    else if(!(strcmp(opcode_str, "not"))){not_f();}
    else if(!(strcmp(opcode_str, "ret"))){ret_f();}
    else if(!(strcmp(opcode_str, "lshf"))){lshf_f();}
    else if(!(strcmp(opcode_str, "rshfl"))){rshfl_f();}
    else if(!(strcmp(opcode_str, "rshfa"))){rshfa_f();}
    else if(!(strcmp(opcode_str, "rti"))){rti_f();}
    else if(!(strcmp(opcode_str, "stb"))){stb_f();}
    else if(!(strcmp(opcode_str, "stw"))){stw_f();}
    else if(!(strcmp(opcode_str, "trap"))){trap_f();}
    else if(!(strcmp(opcode_str, "xor"))){xor_f();}

    else if(!(strcmp(opcode_str, "br"))){return 1;}
    else if(!(strcmp(opcode_str, "brn"))){return 1;}
    else if(!(strcmp(opcode_str, "brp"))){return 1;}
    else if(!(strcmp(opcode_str, "brnp"))){return 1;}
    else if(!(strcmp(opcode_str, "brz"))){return 1;}
    else if(!(strcmp(opcode_str, "brnz"))){return 1;}
    else if(!(strcmp(opcode_str, "brzp"))){return 1;}
    else if(!(strcmp(opcode_str, "brnzp"))){return 1;}
}



/*------------HELPER FUNCTIONS------------*/
//Convert array of 1's and 0's to 
//Converts strings of 1's and 0's to hex 

//e^t
int pow(int e, int t){
    if(!t) {return 1;}

    int result;
    t = t-1;
    for(result = e; t != 0; t--){
        result *= e;
    }
    return result;
}

//Register find and return number associated w register
/*num_arr_flag = 0:return decimal no of reg, 1:add 3bit encoding to int_bit_array starting at start bit*/
int regDecode(char* reg_str, int num_arr_flag, int start_bit){
    int i;
    for(i=0; (strcmp(regDictionary[i],"\0")); i++ ){
        if (!(strcmp(regDictionary[i], reg_str))){
            if(!num_arr_flag) {return i;}
            else{
                int_bin_array[start_bit] = i/4;
                i = i % 4;
                int_bin_array[start_bit - 1] = i/2;
                i = i % 2;
                int_bin_array[start_bit - 2] = i;
                return -1;
            }
        }
    }
    return -1;
}

//Expects packed "int_bin_array" and converts to decimals and puts into hex variables
void arrToHex(){
   int i;
   int decimal = 0;

   for(i=0; i<4; i++){decimal += int_bin_array[i] * pow(2, i);}
   hex0 = decimal;
   decimal = 0;
   for(i=0; i<4; i++){decimal += int_bin_array[i+4] * pow(2, i);}
   hex1 = decimal;
   decimal = 0;
   for(i=0; i<4; i++){decimal += int_bin_array[i+8] * pow(2, i);}
   hex2 = decimal;
}

void testPrint_intArr(){
    int i;
    for (i=11; i > -1; i--){
        printf("%d", int_bin_array[i]);
    }
    printf("\n");
}

/*Convert string to number
Hex numbers must be in the form “x3000”, and decimal numbers must be in the form “#30”.*/
int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}