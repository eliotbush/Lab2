//
//  main.c
//  Lab2
//
//  Created by Emma Bryce on 10/17/15.
//  Copyright © 2015 Emma Bryce. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool verifyInstruction(char **);
bool verifyRegister(char *);
char* translateRegister(char *);
bool verifyAddress(char *);
bool verifyImmediate(char *);
struct inst convertInstruction(char **);

/////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {ADD, SUB, ADDI, MUL, LW, SW, BEQ} OPCODE;

struct latch {
    bool consumerFlag;
    bool producerFlag;
    int operandOne;
    int operandTwo;
    int destRegister;
    OPCODE opcode;
};

struct inst {
    OPCODE opcode;
    int rs;
    int rt;
    int rd;
    int immediate;
};
    

/////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, const char * argv[]) {
    struct inst *instructionMemory;
    instructionMemory = (struct inst *) malloc(512*sizeof(struct inst));
    char *line;
    line = (char *) malloc(100*sizeof(char));
    char delimiters[] = ", ";
    int i;
    int j;

    FILE *assembly_program = fopen("assemblyProgram.txt", "rt");
    
    j=0;
    char *token;
    token = (char * ) malloc(100*sizeof(char));
    while (fgets(line, 100, assembly_program) != NULL){
        char **instructionString;
        instructionString = (char **) malloc(10*sizeof(char*));
	for(i=0; i<10; i++){instructionString[i] = NULL;}
        i=0;
        token = strtok(line, delimiters);
        while(token != NULL){
            instructionString[i] = strdup(token);
            token = strtok(NULL, delimiters);
            i++;
        }
        //printf("%s %s %s %s\n", instructionString[0], instructionString[1], instructionString[2], instructionString[3]);
        //still working on verifyInstruction.
        //we need to test convertInstruction function
        //instructionMemory[j] = convertInstruction(instructionString);
        j++;
    }
    for (i=0; i<j; i++){printf("%d %d %d %d %d\n", instructionMemory[i].opcode, instructionMemory[i].rs, instructionMemory[i].rt, instructionMemory[i].rd, instructionMemory[i].immediate);}
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//converts a instruction in the form of tokens into a struct
struct inst convertInstruction(char **instr){
    struct inst output;

    //convert opcode field
    if (strncmp(instr[0], "add", 10)==0){output.opcode = ADD;}
    else if (strncmp(instr[0], "sub", 10)==0){output.opcode = SUB;}
    else if (strncmp(instr[0], "addi", 10)==0){output.opcode = ADDI;}
    else if (strncmp(instr[0], "mul", 10)==0){output.opcode = MUL;}
    else if (strncmp(instr[0], "lw", 10)==0){output.opcode = LW;}
    else if (strncmp(instr[0], "sw", 10)==0){output.opcode = SW;}
    else if (strncmp(instr[0], "beq", 10)==0){output.opcode = BEQ;}

    //convert rs and rt
    char delimiters[] = "$\n";
    char *regStr;
    regStr = strtok(translateRegister(instr[1]), delimiters);
    sscanf(regStr, "%i", &output.rs);

    //addi or beq
    if ((strncmp(instr[0], "addi", 10)==0) || (strncmp(instr[0], "beq", 10)==0)) {
        regStr = strtok(translateRegister(instr[2]), delimiters);
        sscanf(regStr, "%i", &output.rt);
        regStr = strtok(instr[3], delimiters);
        sscanf(regStr, "%i", &output.immediate);
    }

    //lw or sw
    else if((strncmp(instr[0], "lw", 10)==0) || (strncmp(instr[0], "sw", 10)==0)) {
        char delimiters2[] = "()";
        regStr = strtok(instr[2], delimiters2);
        sscanf(regStr, "%i", &output.immediate);
        regStr = strtok(NULL, delimiters2);
        regStr = translateRegister(regStr);
        regStr = strtok(regStr, delimiters);
        sscanf(regStr, "%i", &output.rt);
    }

    //rd
    else {
        regStr = strtok(translateRegister(instr[2]), delimiters);
        sscanf(regStr, "%i", &output.rt);
        regStr = strtok(translateRegister(instr[3]), delimiters);
        sscanf(regStr, "%i", &output.rd);
    }
    return output;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//verifies whether an instruction (token array) is kosher or not
//returns a bool: use it directly in conditionals/asserts
bool verifyInstruction(char **instr){
    //add instruction
    if(strncmp(instr[0], "add", 10)==0){
        //verify arg count
        if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
            printf("bad arg count for add\n");
            return false;
        }
        //call other functions to translate/verify registers
        //if they all check out, return true (nested AND).
        //the verify/translate register functions will print the appropriate error messages if they fail
        else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyRegister(translateRegister(instr[3])));}
    }
    
    //sub instruction: same process as add
    else if(strncmp(instr[0], "sub", 10)==0){
        if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
            printf("bad arg count for sub\n");
            return false;
        }
        else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyRegister(translateRegister(instr[3])));}
    }
    
    //addi instruction.
    //need some clarification on how to verify immediates, right now it just always returns true if there's something in the imm field
    else if(strncmp(instr[0], "addi", 10)==0){
        if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
            printf("bad arg count for addi\n");
            return false;
        }
        else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyImmediate(instr[3]));}
    }
    
    //mul instruction
    else if(strncmp(instr[0], "mul", 10)==0){
        if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
            printf("bad arg count for mul\n");
            return false;
        }
        else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyRegister(translateRegister(instr[3])));}
    }
    
    //lw
    else if(strncmp(instr[0], "lw", 10)==0){
        if(instr[3]!=NULL || instr[2]==NULL || instr[1]==NULL){
            printf("bad arg count for lw\n");
            return false;
        }
        //here verifyAddress is used
        else{return (verifyRegister(translateRegister(instr[1])) && verifyAddress(instr[2]));}
    }
    
    //sw (same as lw)
    else if(strncmp(instr[0], "sw", 10)==0){
        if(instr[3]!=NULL || instr[2]==NULL || instr[1]==NULL){
            printf("bad arg count for sw\n");
            return false;
        }
        else{return (verifyRegister(translateRegister(instr[1])) && verifyAddress(instr[2]));}
    }
    
    //beq
    else if(strncmp(instr[0], "beq", 10)==0){
        if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
            printf("bad arg count for beq\n");
            return false;
        }
        else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyAddress(instr[3]));}
    }
    
    //didn't find valid opcode!
    else{
        printf("bad opcode: %s\n", instr[0]);
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

//verify a NUMERICAL valued register.
bool verifyRegister(char *Reg){
    char *c;
    c = "\n";
    char *reg;
    reg = strtok(Reg, c);
    if(reg[0]=='$'){
        //register $0-9
        //check if 0 <= reg[1] <= 9
        if((reg[1]-'0')>=0 && (reg[1]-'0')<=9 && strlen(reg)==2){return true;}
        
        //registers $10-$19
        else if(reg[1]=='1' && strlen(reg)==3){
            //check if 0 <= reg[2] <= 9
            if((reg[2]-'0')>=0 && (reg[2]-'0')<=9){return true;}
            else{
                printf("bad register declaration: %s\n", reg);
                return false;
            }
        }
        
        //registers $20-$29
        else if(reg[1]=='2' && strlen(reg)==3){
            //check if 0 <= reg[2] <= 9
            if((reg[2]-'0')>=0 && (reg[2]-'0')<=9){return true;}
            else{
                printf("bad register declaration: %s\n", reg);
                return false;
            }
        }
        
        //registers $30-$31
        else if(reg[1]=='3' && strlen(reg)==3){
            //check if 0 <= reg[2] <= 1
            if((reg[2]-'0')>=0 && (reg[2]-'0')<=1){return true;}
            else{
                printf("bad register declaration: %s\n", reg);
                return false;
            }
        }
        
        else{
            printf("bad register declaration: %s\n", reg);
            return false;
        }
    }
    
    else{
        printf("bad register declaration: %s\n", reg);
        return false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

//translates a symbolic register into a numerical one
//for unrecognized symbols (either an illegal symbol or a numerical register) it returns the original input
//so numeric registers just pass through, and invalid symbolic will trip the verifyRegister stage
char* translateRegister(char *Reg){
    char *c;
    c = "\n";
    char *reg;
    reg = strtok(Reg, c);
    
    //$zero
    if(strncmp(reg, "$zero", 10)==0){return "$0";}
    
    //$gp
    else if(strncmp(reg, "$gp", 10)==0){return "$28";}
    
    //$fp
    else if(strncmp(reg, "$fp", 10)==0){return "$30";}
    
    //$ra
    else if(strncmp(reg, "$ra", 10)==0){return "$31";}
    
    //$s*
    //if the first two bytes of reg are $s...
    else if(strncmp(reg, "$s", 2)==0){
        //$s0-$s7
        //if 0<=reg[2]<=7
        if((reg[2]-'0')>=0 && (reg[2]-'0')<=7 && strlen(reg)==3){
            char *buffer1, *buffer2;
            //temp strings
            buffer1 = (char *) malloc(3*sizeof(char));
            buffer2 = (char *) malloc(2*sizeof(char));
            //put reg[2] into buffer2 as a string.
            //16 is added because $s0 = $16
            sprintf(buffer2, "%d", ((reg[2]-'0')+16));
            //make buffer1 "$"
            strcpy(buffer1, "$");
            //concatenate "$" with the numerical reg value
            strcat(buffer1, buffer2);
            return buffer1;
            
        }
        //$sp
        else if(strncmp(reg, "$sp", 10)==0){return "$29";}
        else{return reg;}
    }
    
    //$t*
    //same process as $s*, just different bounds for the checks
    else if(strncmp(reg, "$t", 2)==0){
        //$t0-$t7
        if((reg[2]-'0')>=0 && (reg[2]-'0')<=7 && strlen(reg)==3){
            char *buffer1, *buffer2;
            buffer1 = (char *) malloc(3*sizeof(char));
            buffer2 = (char *) malloc(2*sizeof(char));
            //add 8 because $t0 = $8
            sprintf(buffer2, "%d", ((reg[2]-'0')+8));
            strcpy(buffer1, "$");
            strcat(buffer1, buffer2);
            return buffer1;
            
        }
        //$t8-$t9
        else if((reg[2]-'0')>=8 && (reg[2]-'0')<=9 && strlen(reg)==3){
            char *buffer1, *buffer2;
            buffer1 = (char *) malloc(3*sizeof(char));
            buffer2 = (char *) malloc(2*sizeof(char));
            //$t8 = 24, so add 16
            sprintf(buffer2, "%d", ((reg[2]-'0')+16));
            strcpy(buffer1, "$");
            strcat(buffer1, buffer2);
            return buffer1;
            
        }
        else{return reg;}
    }
    
    //$a*
    //same idea as before
    else if(strncmp(reg, "$a", 2)==0){
        //$a0-$a7
        if((reg[2]-'0')>=0 && (reg[2]-'0')<=3 && strlen(reg)==3){
            char *buffer1, *buffer2;
            buffer1 = (char *) malloc(3*sizeof(char));
            buffer2 = (char *) malloc(2*sizeof(char));
            //$a0=$4
            sprintf(buffer2, "%d", ((reg[2]-'0')+4));
            strcpy(buffer1, "$");
            strcat(buffer1, buffer2);
            return buffer1;
            
        }
        //$at
        else if(strncmp(reg, "$at", 10)==0){return "$1";}
        else{return reg;}
    }
    
    //$k*
    else if(strncmp(reg, "$k", 2)==0){
        //$k0-$k1
        if((reg[2]-'0')>=0 && (reg[2]-'0')<=1 && strlen(reg)==3){
            char *buffer1, *buffer2;
            buffer1 = (char *) malloc(3*sizeof(char));
            buffer2 = (char *) malloc(2*sizeof(char));
            //$k0=$26
            sprintf(buffer2, "%d", ((reg[2]-'0')+26));
            strcpy(buffer1, "$");
            strcat(buffer1, buffer2);
            return buffer1;
            
        }
        else{return reg;}
    }
    
    //$v*
    else if(strncmp(reg, "$v", 2)==0){
        //$v0-$v1
        if((reg[2]-'0')>=0 && (reg[2]-'0')<=1 && strlen(reg)==3){
            char *buffer1, *buffer2;
            buffer1 = (char *) malloc(3*sizeof(char));
            buffer2 = (char *) malloc(2*sizeof(char));
            //$v0=$2
            sprintf(buffer2, "%d", ((reg[2]-'0')+2));
            strcpy(buffer1, "$");
            strcat(buffer1, buffer2);
            return buffer1;
            
        }
        else{return reg;}
    }
    
    else{return reg;}
}

////////////////////////////////////////////////////////////////////////////////////

//verify that an address is valid
//returns bool for conditionals
bool verifyAddress(char *Addr){
    char *c;
    c = "\n";
    char *addr;
    addr = strtok(Addr, c);
    
    //if the first char isn't a number quit out
    if((addr[0]-'0')<0 || (addr[0]-'0')>9){printf("bad address: %s", addr); return false;}
    
    int i=0;
    //while addr[i] is a number
    while((addr[i]-'0')>=0 && (addr[i]-'0')<=9){i++;}
    
    //if addr[i] isn't (
    if(addr[i]!='('){printf("bad address: %s\n", addr); return false;}
    
    else{
        //make a copy (strtok will mess up addr if we don't)
        char *addrCopy;
        addrCopy = (char *) malloc(100*sizeof(char));
        strcpy(addrCopy, addr);
        //delimiter for strtok
        char delimiter[] = "()";
        //token1 is junk
        char *token1;
        //token2 should (hopefully) hold the register name inside offset($reg)
        char *token2;
        token1 = (char *) malloc(100*sizeof(char));
        token2 = (char *) malloc(100*sizeof(char));
        //cut up addrCopy
        token1 = strtok(addrCopy, delimiter);
        //this is the actual register
        token2 = strtok(NULL, delimiter);
        //if there's other stuff after the ) delimiter, the address is bad
        if(strlen(addr)>(i+strlen(token2)+2)){printf("bad address: %s\n", addr); return false;}
        //if the register is legal and the last character was a ')' (otherwise it could be '(') it's valid
        else if(verifyRegister(translateRegister(token2)) && addr[strlen(addr)-1]==')'){return true;}
        //otherwise not
        else{printf("bad address: %s\n", addr); return false;}
    }
}

/////////////////////////////////////////////////////////////////////////////////////

//need instructor clarification on this. should it just be a number?
//test print statements are commented out
bool verifyImmediate(char *imm){
int i;
char * neg;
neg = (char *) malloc(((strlen(imm))-1)*sizeof(char));
int value;
bool isNeg = false;
bool isNumber = true;
bool isBit = false;
//preliminary stage: is it a number?
for (i=0; i<strlen(imm); i++){
if((imm[i] - '0')>9 || imm[i]- '0'<0){
isNumber = false;
}
}
//is it a negative number?
if(imm[0] == '-'){
isNeg = true;
isNumber = true;
for (i = 1; i<strlen(imm); i++){
if((imm[i] - '0')>9 || imm[i] - '0'<0){
isNumber = false;
}
}
}
//0 case
if((imm[0] - '0') == 0){
isNumber = true;
for (i = 1; i<strlen(imm); i++){
if((imm[i] - '0')>9 || (imm[i] - '0')<0){
isNumber = false;
}
}
}
//If it is a negative number than put the value into a pointer and convert that to an int
if(isNeg&&isNumber){
for(i = 1; i<strlen(imm); i++){
neg[i-1] = imm[i];
}
value = atoi(neg);
}
//Otherwise convert from the input pointer
if(!isNeg&&isNumber){value = atoi(imm);}

//This is for those weird cases

if(value < 0){isBit = false;}


//isBit Test goes here
if(isNumber){
	if(32768-value > 0){
        isBit = true;
}
}
if(isNumber && isNeg){
	if(value == 32768){
		isBit = true;
	}
}
return (isNumber&&isBit);
}




////////////////////////////////////////////////////////////////////////////////////
