/*
#include <string.h>
#include <math.h>
#include <assert.h>
//feel free to add here any additional library names you may need 
#define SINGLE 1
#define BATCH 0
#define REG_NUM 32
main (int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long mips_reg[REG_NUM];
	long pgm_c=0;//program counter
	long sim_cycle=0;//simulation cycle counter
	//define your own counter for the usage of each pipeline stage here
	
	int test_counter=0;
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for(i=1;i<argc;i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
	if(argc==7){
		if(strcmp("-s",argv[1])==0){
			sim_mode=SINGLE;
		}
		else if(strcmp("-b",argv[1])==0){
			sim_mode=BATCH;
		}
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");
		
	}
	
	else{
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
		printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
		exit(0);
	}
	if(input==NULL){
		printf("Unable to open input or output file\n");
		exit(0);
	}
	if(output==NULL){
		printf("Cannot create output file\n");
		exit(0);
	}
	//initialize registers and program counter
	if(sim_mode==1){
		for (i=0;i<REG_NUM;i++){
			mips_reg[i]=0;
		}
	}
	
	//start your code from here
}
*/

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
    

/////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, const char * argv[]) {
    int i;
    int j;
    int sim_mode=1;
    m=2;
    n=3;
    c=3;
    mips_reg = (int *) malloc(32*sizeof(int));
    for(i=0; i<32; i++){mips_reg[i] = 0;}
    registerFlags = (bool *) malloc(32*sizeof(bool));
    for(i=0; i<32; i++){registerFlags[i] = true;}
    dataMemory = (int *) malloc(512*sizeof(int));
    for(i=0; i<512; i++){dataMemory[i] = 0;}

    instructionMemory = (struct inst *) malloc(512*sizeof(struct inst));
    for(i=0; i<512; i++){instructionMemory[i].opcode = HALT;}
    char *line;
    line = (char *) malloc(100*sizeof(char));
    char delimiters[] = ", ";

    FILE *assembly_program = fopen("assemblyProgram.txt", "rt");
    
    j=0;
    char *token;
    token = (char * ) malloc(100*sizeof(char));
    while (fgets(line, 100, assembly_program) != NULL){
        char **instructionString;
        instructionString = (char **) malloc(10*sizeof(char*));
	for(i=0; i<10; i++){
            instructionString[i] = (char *) malloc(100*sizeof(char));
            instructionString[i] = NULL;
        }
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
        instructionMemory[j] = convertInstruction(instructionString);
        j++;
    }
    for (i=0; i<j; i++){printf("%d %d %d %d %d\n\n", instructionMemory[i].opcode, instructionMemory[i].rs, instructionMemory[i].rt, instructionMemory[i].rd, instructionMemory[i].immediate);}
    printf("true: %d\n\n", true);

    initializeLatches();

    printf("(IF) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d instr_rs: %d instr_rt: %d instr_rd: %d instr_imm: %d\n", IF.flag, IF.operandOne, IF.operandTwo, IF.destRegister, IF.counter, IF.opcode, IF.instruction.opcode, IF.instruction.rs, IF.instruction.rt, IF.instruction.rd, IF.instruction.immediate);
    printf("(IF_ID) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d instr_rs: %d instr_rt: %d instr_rd: %d instr_imm: %d\n", IF_ID.flag, IF_ID.operandOne, IF_ID.operandTwo, IF_ID.destRegister, IF_ID.counter, IF_ID.opcode, IF_ID.instruction.opcode, IF_ID.instruction.rs, IF_ID.instruction.rt, IF_ID.instruction.rd, IF_ID.instruction.immediate);
    printf("(ID) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d instr_rs: %d instr_rt: %d instr_rd: %d instr_imm: %d\n", ID.flag, ID.operandOne, ID.operandTwo, ID.destRegister, ID.counter, ID.opcode, ID.instruction.opcode, ID.instruction.rs, ID.instruction.rt, ID.instruction.rd, ID.instruction.immediate);
    printf("(ID_EX) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", ID_EX.flag, ID_EX.operandOne, ID_EX.operandTwo, ID_EX.destRegister, ID_EX.counter, ID_EX.opcode, ID_EX.instruction.opcode);
    printf("(EX) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", EX.flag, EX.operandOne, EX.operandTwo, EX.destRegister, EX.counter, EX.opcode, EX.instruction.opcode);
    printf("(EX_MEM) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", EX_MEM.flag, EX_MEM.operandOne, EX_MEM.operandTwo, EX_MEM.destRegister, EX_MEM.counter, EX_MEM.opcode, EX_MEM.instruction.opcode);
    printf("(MEM) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", MEM.flag, MEM.operandOne, MEM.operandTwo, MEM.destRegister, MEM.counter, MEM.opcode, MEM.instruction.opcode);
    printf("(MEM_WB) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", MEM_WB.flag, MEM_WB.operandOne, MEM_WB.operandTwo, MEM_WB.destRegister, MEM_WB.counter, MEM_WB.opcode, MEM_WB.instruction.opcode);
    printf("(WB) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n\n", WB.flag, WB.operandOne, WB.operandTwo, WB.destRegister, WB.counter, WB.opcode, WB.instruction.opcode);
    while(isRunning){
        WB_stage();
        MEM_stage();
        EX_stage();
        ID_stage();
        IF_stage();
        printf("(IF) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d instr_rs: %d instr_rt: %d instr_rd: %d instr_imm: %d\n", IF.flag, IF.operandOne, IF.operandTwo, IF.destRegister, IF.counter, IF.opcode, IF.instruction.opcode, IF.instruction.rs, IF.instruction.rt, IF.instruction.rd, IF.instruction.immediate);
        printf("(IF_ID) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d instr_rs: %d instr_rt: %d instr_rd: %d instr_imm: %d\n", IF_ID.flag, IF_ID.operandOne, IF_ID.operandTwo, IF_ID.destRegister, IF_ID.counter, IF_ID.opcode, IF_ID.instruction.opcode, IF_ID.instruction.rs, IF_ID.instruction.rt, IF_ID.instruction.rd, IF_ID.instruction.immediate);
        printf("(ID) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d instr_rs: %d instr_rt: %d instr_rd: %d instr_imm: %d\n", ID.flag, ID.operandOne, ID.operandTwo, ID.destRegister, ID.counter, ID.opcode, ID.instruction.opcode, ID.instruction.rs, ID.instruction.rt, ID.instruction.rd, ID.instruction.immediate);
        printf("(ID_EX) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", ID_EX.flag, ID_EX.operandOne, ID_EX.operandTwo, ID_EX.destRegister, ID_EX.counter, ID_EX.opcode, ID_EX.instruction.opcode);
        printf("(EX) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", EX.flag, EX.operandOne, EX.operandTwo, EX.destRegister, EX.counter, EX.opcode, EX.instruction.opcode);
        printf("(EX_MEM) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", EX_MEM.flag, EX_MEM.operandOne, EX_MEM.operandTwo, EX_MEM.destRegister, EX_MEM.counter, EX_MEM.opcode, EX_MEM.instruction.opcode);
        printf("(MEM) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", MEM.flag, MEM.operandOne, MEM.operandTwo, MEM.destRegister, MEM.counter, MEM.opcode, MEM.instruction.opcode);
        printf("(MEM_WB) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n", MEM_WB.flag, MEM_WB.operandOne, MEM_WB.operandTwo, MEM_WB.destRegister, MEM_WB.counter, MEM_WB.opcode, MEM_WB.instruction.opcode);
        printf("(WB) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d instropcode: %d\n\n", WB.flag, WB.operandOne, WB.operandTwo, WB.destRegister, WB.counter, WB.opcode, WB.instruction.opcode);
	//output code 2: the following code will output the register 
        //value to screen at every cycle and wait for the ENTER key
        //to be pressed; this will make it proceed to the next cycle 
	printf("cycle: %d ",sim_cycle);
	if(sim_mode==1){
		for (i=1;i<REG_NUM;i++){
			printf("%d  ",mips_reg[i]);
		}
                printf("%d\n",pgm_c);
	        printf("cycle: %d ",sim_cycle);
		for (i=1;i<REG_NUM;i++){
			printf("%d  ",registerFlags[i]);
		}
	}
	printf("%d\n",pgm_c);
	sim_cycle+=1;
	printf("press ENTER to continue\n");
	while(getchar() != '\n');
    }

	return 0;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////

//converts a instruction in the form of tokens into a struct
struct inst convertInstruction(char **instr){
    struct inst output;
    char *copy;

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
        copy = strdup(translateRegister(instr[2]));
        regStr = strtok(copy, delimiters);
        sscanf(regStr, "%i", &output.rs);
        copy = strdup(translateRegister(instr[1]));
        regStr = strtok(copy, delimiters);
        sscanf(regStr, "%i", &output.rt);
        copy = strdup(instr[3]);
        regStr = strtok(copy, delimiters);
        sscanf(regStr, "%i", &output.immediate);
    }

    //lw or sw
    else if((strncmp(instr[0], "lw", 10)==0) || (strncmp(instr[0], "sw", 10)==0)) {
        char delimiters2[] = "()";
        copy = strdup(translateRegister(instr[1]));
        regStr = strtok(copy, delimiters);
        sscanf(regStr, "%i", &output.rt);
        copy = strdup(instr[2]);
        regStr = strtok(copy, delimiters2);
        sscanf(regStr, "%i", &output.immediate);
        regStr = strtok(NULL, delimiters2);
        regStr = translateRegister(regStr);
        regStr = strtok(regStr, delimiters);
        sscanf(regStr, "%i", &output.rs);
    }

    //rd
    else {
        copy = strdup(translateRegister(instr[1]));
        regStr = strtok(copy, delimiters);
        sscanf(regStr, "%i", &output.rd);
        copy = strdup(translateRegister(instr[2]));
        regStr = strtok(copy, delimiters);
        sscanf(regStr, "%i", &output.rs);
        copy = strdup(translateRegister(instr[3]));
        regStr = strtok(copy, delimiters);
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




////////////////////////////////////////////////////////////////////////////////////

void MEM_stage(void){
    //check the counter: if it's not 0, we're in the middle of a memory access
    if (MEM.counter>1){
        MEM.counter--;
        MEM_util++;
    }

    //if MEM is ready for an instruction and EX is done
    else if (MEM.counter==0 && EX_MEM.flag){
        //copy EXMEM latch into internal MEM latch
        MEM = EX_MEM;
        //set the EXMEM flag to "used"
        EX_MEM.flag = false;
        MEM_util++;
        //if it's lw or sw
        if ((MEM.opcode==LW) || (MEM.opcode==SW)){
            if (c>1){
                //start the counter
                MEM.counter = c-1;
                //if the address is bad, stop execution
                if ((MEM.operandOne<0)||(MEM.operandOne>2048)||(MEM.operandOne % 4 != 0)){
                    printf("Invalid address: %d\n", MEM.operandOne);
                    assert((MEM.operandOne>=0)&&(MEM.operandOne<=2048)&&(MEM.operandOne % 4 == 0));
                }
            }
            //if c==1
            else {
                //if SW, write the register contents into data mem
                if (MEM.opcode==SW){dataMemory[MEM.operandOne/4] = MEM.destRegister;}
                //if LW, fetch mem contents and put it in the operand field, then pass it on to WB
                else {
                    MEM.operandOne = dataMemory[MEM.operandOne/4];
                    MEM_WB = MEM;
                }
            }
        }
        else if(MEM.opcode==HALT){
            MEM_util--;
            MEM_WB = MEM;
        }
        //otherwise just pass the instruction through to WB
        else{MEM_WB = MEM;}
            
    }

    //if we're on the last step of the countdown
    else if (MEM.counter==1){
        MEM_util++;
        MEM.counter--;
        //if SW, write the register contents into data mem
        if (MEM.opcode==SW){dataMemory[MEM.operandOne/4] = MEM.destRegister;}
        //if LW, fetch mem contents and put it in the operand field, then pass it on to WB
        else {
            MEM.operandOne = dataMemory[MEM.operandOne/4];
            MEM_WB = MEM;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void IF_stage(void){
    //if we're in the middle of a memory access
    if(IF.counter>1){
        IF.counter--;
        IF_util++;
    }

    //if ID is ready for a new instruction and counter's 1
    else if ((IF.counter==1)&&(IF_ID.flag==false)) {
        IF_util++;
        assert((pgm_c%4==0)&&(pgm_c>=0));
        IF.instruction = instructionMemory[pgm_c/4];
        IF.counter--;
        branchFlag =  ((IF.instruction.opcode==BEQ)||(IF.instruction.opcode==HALT));
        IF_ID = IF;
        IF_ID.flag = true;
        pgm_c = pgm_c+4;
    }

    //if IF is ready for a new instruction and there are no unresolved brnaches
    else if ((IF.counter==0)&&(branchFlag==false)){
        if(c>1){
            IF.counter=c-1;
            IF_util++;
        }
	else if (IF_ID.flag){
            IF_util++;
            assert((pgm_c>=0)&&(pgm_c%4==0));
            IF.instruction = instructionMemory[pgm_c/4];
            branchFlag =  (IF.instruction.opcode==BEQ);
            pgm_c = pgm_c+4;
            IF_ID = IF;
            IF_ID.flag = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////

void ID_stage(void){
    if (ID.flag && IF_ID.flag){
        ID = IF_ID;
        IF_ID.flag = false;
        ID_util++;
        if (ID.instruction.opcode==ADD || ID.instruction.opcode==SUB || ID.instruction.opcode==MUL){
            if (registerFlags[ID.instruction.rs] && registerFlags[ID.instruction.rt]){
                ID.operandOne = mips_reg[ID.instruction.rs];
                ID.operandTwo = mips_reg[ID.instruction.rt];
                registerFlags[ID.instruction.rd] = false;
                ID.destRegister = ID.instruction.rd;
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag){ID.flag=false;}
                else{ID_EX = ID;}
            }
            else{ID.flag = false;}
        }
        else if (ID.instruction.opcode==ADDI){
            if (registerFlags[ID.instruction.rs]){
                ID.operandOne = mips_reg[ID.instruction.rs];
                ID.operandTwo = ID.instruction.immediate;
                ID.destRegister = ID.instruction.rt;
                registerFlags[ID.instruction.rt] = false;
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag){ID.flag=false;}
                else{ID_EX = ID;}
            }
            else{ID.flag = false;}
        }
        else if (ID.instruction.opcode==BEQ){
            if (registerFlags[ID.instruction.rs] && registerFlags[ID.instruction.rt]){
                ID.operandTwo = mips_reg[ID.instruction.rs];
                ID.operandOne = ID.instruction.immediate;
                ID.destRegister = mips_reg[ID.instruction.rt];
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag){ID.flag=false;}
                else{ID_EX = ID;}
            }
            else{ID.flag = false;}
        }
        else if (ID.instruction.opcode==LW || ID.instruction.opcode==SW){
            if (registerFlags[ID.instruction.rs] && registerFlags[ID.instruction.rt]){
                ID.operandTwo = mips_reg[ID.instruction.rs];
                ID.operandOne = ID.instruction.immediate;
                if(ID.instruction.opcode==SW){
                    ID.destRegister = mips_reg[ID.instruction.rt];}
                else{
                    ID.destRegister = ID.instruction.rt;
                    registerFlags[ID.instruction.rt] = false;
                }
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag){ID.flag=false;}
                else{ID_EX = ID;}
            }
            else{ID.flag = false;}
        }
	else if (ID.instruction.opcode==HALT){
            ID.opcode = HALT;
            ID_util--;
            if(ID_EX.flag){ID.flag=false;}
            else{ID_EX = ID;}
        }
        
    }

    else if(ID.flag==false){
        if (ID.instruction.opcode==ADD || ID.instruction.opcode==SUB || ID.instruction.opcode==MUL){
            if (registerFlags[ID.instruction.rs]==true && registerFlags[ID.instruction.rt]==true){
                ID.flag = true;
                ID.operandOne = mips_reg[ID.instruction.rs];
                ID.operandTwo = mips_reg[ID.instruction.rt];
                registerFlags[ID.instruction.rd];
                ID.destRegister = ID.instruction.rd;
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag==false){
                    ID_EX = ID;
                    ID_util++;
                }
            }
        }
        else if (ID.instruction.opcode==ADDI){
            if (registerFlags[ID.instruction.rs]){
                ID.flag = true;
                ID.operandOne = mips_reg[ID.instruction.rs];
                ID.operandTwo = ID.instruction.immediate;
                ID.destRegister = ID.instruction.rt;
                registerFlags[ID.instruction.rt] = false;
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag==false){
                    ID_EX = ID;
                    ID_util++;
                }
            }
        }
        else if (ID.instruction.opcode==BEQ){
            if (registerFlags[ID.instruction.rs] && registerFlags[ID.instruction.rt]){
                ID.flag = true;
                ID.operandTwo = mips_reg[ID.instruction.rs];
                ID.operandOne = ID.instruction.immediate;
                ID.destRegister = mips_reg[ID.instruction.rt];
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag==false){
                    ID_EX = ID;
                    ID_util++;
                }
            }
        }
        else if (ID.instruction.opcode==LW || ID.instruction.opcode==SW){
            if (registerFlags[ID.instruction.rs] && registerFlags[ID.instruction.rt]){
                ID.flag = true;
                ID.operandTwo = mips_reg[ID.instruction.rs];
                ID.operandOne = ID.instruction.immediate;
                if(ID.instruction.opcode==SW){ID.destRegister = mips_reg[ID.instruction.rt];}
                else{
                    ID.destRegister = ID.instruction.rt;
                    registerFlags[ID.instruction.rt] = false;
                }
                ID.opcode = ID.instruction.opcode;
                if(ID_EX.flag==false){
                    ID_EX = ID;
                    ID_util++;
                }
            }
        }
	else if (ID.instruction.opcode==HALT){
            if(ID_EX.flag){
                ID.flag=true;
                ID_EX = ID;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////


void WB_stage(void){
    //If the latch has a fresh instruction, do a write back for instructions which write to the register
    //file.
    if(MEM_WB.flag == true){
        if(MEM_WB.opcode == ADDI || MEM_WB.opcode == ADD || MEM_WB.opcode == SUB || MEM_WB.opcode == MUL || MEM_WB.opcode == LW){
            if(MEM_WB.destRegister != 0){
            	WB_util++;
                mips_reg[MEM_WB.destRegister] = MEM_WB.operandOne;
                registerFlags[MEM_WB.destRegister] = true;
                MEM_WB.flag = false;
            }
        }
        else if(MEM_WB.opcode==HALT){isRunning = false;}
        //if the instruction didn't require write back, just reset the producer latch flag and increment
        //utilization.
        else{
            MEM_WB.flag = false;
            WB_util++;
        }
    }       
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void EX_stage(){
    bool ct = true; //prevents me from having to use else and changing indentation
    //When the counter gets down to 1, if the EX_MEM latch is open, do the operation and dump the
    //result into the latch. If the consumer latch is not available, the block is skipped, the function
    //returns, and the counter stays at 1 for the next function call.
    if(EX.counter == 1){
        ct = false;
        if(EX_MEM.flag == false){
            EX.counter--;
            EX_util++;
            if(EX.opcode == MUL){
                EX_MEM.operandOne = EX.operandOne*EX.operandTwo;
                EX_MEM.flag = true;
                EX_MEM.opcode = EX.opcode;
                EX_MEM.destRegister = EX.destRegister;
            }
            if(EX.opcode == SUB){
                EX_MEM = EX;
                EX_MEM.operandOne = EX.operandOne - EX.operandTwo;
                EX_MEM.flag = true;
            }
            if(EX.opcode == BEQ){
                if(EX.destRegister != EX.operandTwo)
                    branchFlag = false;
                else{
                    pgm_c = pgm_c + EX.operandOne*4;
                    branchFlag = false;
                }
            }
            else{
                EX_MEM = EX;
                EX_MEM.operandOne = EX.operandOne + EX.operandTwo;
                EX_MEM.flag = true;
            }
        }
        
    }
    //If EX stage is in the middle of an operation and the counter for the stage is greater than 1,
    //increment the utilization for EX and decrement the counter so that we know EX was doing work and so
    //the operation advances.
    if(EX.counter > 1){
        EX.counter--;
        EX_util++;
    }
    
    //If EX has completed its operation but the producer latch (ID_EX) doesn't have a fresh instruction,
    //"do nothing" /void operation
    else
        if (EX.counter == 0 && ID_EX.flag == false && ct == true)
            ;
    
    //If EX has completed its operation and the producer latch is ready with an instruction, pull in
    //the instruction to an internal latch, and set the flag on the producer latch to false which means it
    //can be overwritten by the producer, ID.
    
    //The counter for the stage will be set to one less than m or n, as it has completed one cycle of the
    //new instruction by the end of this function call.
    
    //The special case of m or n being equal to 1 is dealt with by completing the operation immediately if
    //the EX_MEM latch is open, later if not. (It puts off the operation by setting counter to 1 and not
    //incrementing the utilization, as it will be incremented later).
    
    //Utilization is incremented when appropriate.
    if (EX.counter == 0 && ID_EX.flag == true && ct == true){
        EX = ID_EX;
        ID_EX.flag = false;
        EX_util++;
        //For multiply, set the counter to m-1 for the next function call, but in the case that m=1, do the
        //multiply if the proceding/consumer latch is open. If not, set the EX.counter to 1 and fall through
        //to the normal ALU block on the next call (the block for the condition EX.counter == 1).
        if(EX.opcode == MUL){
            EX.counter = m-1;
            if(EX.counter == 0){
                //Do multiply in this special case.
                if(EX_MEM.flag == false){
                    //transfer result of MUL and instruction details into EX_MEM latch
                    //set flag to show that a new instruction is available to MEM
                    EX_MEM = EX;
                    EX_MEM.operandOne = EX.operandOne*EX.operandTwo;
                    EX_MEM.flag = true;
                }
                else{
                    EX.counter = 1;
                    EX_util--;
                }
            }
        }

        else if((EX.opcode == HALT)&&(EX_MEM.flag==false)){
            EX_util--;
            EX_MEM = EX;
        }

        else if((EX.opcode == HALT)&&(EX_MEM.flag)){EX_util--;}
        
        //For other ALU operations, set the counter to n-1 for the next function call, but in the case
        //that n=1, do the operation if the consumer latch is open. If not, set the EX.counter to n-1 and
        //fall through to the normal ALU block on the next call (the block for the condition
        //EX.counter == 1).
        else{
            EX.counter = n-1;
            if(EX.counter == 0){
                //Do other arithmetic operation, etc.
                if(EX_MEM.flag == false){
                    if(EX.opcode == SUB){
                        EX_MEM = EX;
                        EX_MEM.operandOne = EX.operandOne - EX.operandTwo;
                        EX_MEM.flag = true;
                    }
                    if(EX.opcode == BEQ){
                        if(EX.destRegister != EX.operandTwo)
                            branchFlag = false;
                        else{
                            pgm_c = pgm_c + EX.operandOne*4;
                            branchFlag = false;
                        }
                    }
                    else{
                        //order of instructions important
                        EX_MEM = EX;
                        EX_MEM.operandOne = EX.operandOne + EX.operandTwo;
                        EX_MEM.flag = true;
                    }
                
                }
                //if the consumer latch was blocked, fall through the the EX.counter=1 condition
                else{
                    EX.counter = 1;
                    EX_util--;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void initializeLatches(void){
    IF_ID.flag = false;
    ID.flag = true;
    ID_EX.flag = false;
    EX_MEM.flag = false;
    MEM_WB.flag = false;
//IF_ID, ID, ID_EX, EX, EX_MEM, MEM, MEM_WB, WB
}
