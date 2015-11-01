

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#define REG_NUM 32

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool verifyInstruction(char **);
bool verifyRegister(char *);
char* translateRegister(char *);
bool verifyAddress(char *);
bool verifyImmediate(char *);
struct inst convertInstruction(char **);
void initializeLatches(void);
void IF_stage(void);
void ID_stage(void);
void EX_stage(void);
void MEM_stage(void);
void WB_stage(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {ADD, SUB, ADDI, MUL, LW, SW, BEQ, HALT} OPCODE;

struct inst {
    OPCODE opcode;
    int rs;
    int rt;
    int rd;
    int immediate;
};

struct latch {
    bool flag;
    int operandOne;
    int operandTwo;
    int destRegister;
    int counter;
    OPCODE opcode;
    struct inst instruction;
};

bool branchFlag=false;
bool isRunning=true;
int pgm_c=0;
int m,n,c;
int sim_cycle;
int IF_util, ID_util, EX_util, MEM_util, WB_util = 0;
int *dataMemory;
int *mips_reg;
bool *registerFlags;
struct inst *instructionMemory;
struct latch IF, IF_ID, ID, ID_EX, EX, EX_MEM, MEM, MEM_WB, WB;
//////////////////////////////////////



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

    //addi or beq
    if ((strncmp(instr[0], "addi", 10)==0) || (strncmp(instr[0], "beq", 10)==0)) {
        regStr = strtok(translateRegister(instr[2]), delimiters);
        sscanf(regStr, "%i", &output.rs);
        regStr = strtok(translateRegister(instr[1]), delimiters);
        sscanf(regStr, "%i", &output.rt);
        regStr = strtok(instr[3], delimiters);
        sscanf(regStr, "%i", &output.immediate);
    }

    //lw or sw
    else if((strncmp(instr[0], "lw", 10)==0) || (strncmp(instr[0], "sw", 10)==0)) {
        char delimiters2[] = "()";
        regStr = strtok(translateRegister(instr[1]), delimiters);
        sscanf(regStr, "%i", &output.rt);
        regStr = strtok(instr[2], delimiters2);
        sscanf(regStr, "%i", &output.immediate);
        regStr = strtok(NULL, delimiters2);
        regStr = translateRegister(regStr);
        regStr = strtok(regStr, delimiters);
        sscanf(regStr, "%i", &output.rs);
    }

    //rd
    else {
        regStr = strtok(translateRegister(instr[1]), delimiters);
        sscanf(regStr, "%i", &output.rd);
        regStr = strtok(translateRegister(instr[2]), delimiters);
        sscanf(regStr, "%i", &output.rs);
        regStr = strtok(translateRegister(instr[3]), delimiters);
        sscanf(regStr, "%i", &output.rt);
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
bool isValid;
char * offset;
char * reg;
int i;
int j;
int k = 0;
int y;

//First tests are to make sure that the address is valid from the start

if((Addr[0] - '0')>9 || Addr[0]- '0'<0){
    return false;
}


if(Addr[(strlen(Addr)-1)]!=')'){
    return false;
}

//After passing the initial tests the for loop iterates through the string from the last index to the first

for(i = strlen(Addr)-1; i>-1;i--){
	//During the backwards traversal it checks for the register's signature ($)
	
        if(Addr[i] == '$'){
        	//if and when this symbol is found then store the register into a character pointer using a for loop.
            reg = (char *) malloc(10*sizeof(char));
            for(j=i; j<strlen(Addr)-1; j++){
                reg[k]=Addr[i+k];
                k++;//this is included because the reference to the new character pointer must be from index zero
                //this iterates through in the middle of the loop and keeps the indices lined up
            }
        
        }
        
        
        
        if(Addr[i] == '('){
        	//if and when the open paren is found store all the characters up to the open paran
        	//into a character pointer reserved for the offset value
        	
            offset = (char *) malloc(10*sizeof(char));
            for(j=0; j<i;j++){
                offset[j]=Addr[j];
            }
        }


}
//the character pointers can now be input into their respective test functions
//there is however a need to translate the reg pointer before a call to verifyRegister is made

if(verifyRegister(translateRegister(reg)) && verifyImmediate(offset)){
    return true;
    
}
return false;    
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
int main(int argc, const char * argv[]) {
return 0;
}
