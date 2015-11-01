//
//  test.c
//  Lab2
//
//  Created by Emma Bryce on 11/1/15.
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
void EX_stage();

/////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {ADD, SUB, ADDI, MUL, LW, SW, BEQ} OPCODE;
void EX_stage();

int PC = 0;
int m;
int n;
int c;
int *registerFile;
bool *registerFlags;
bool branchFlag = false;
int IF_util, ID_util, EX_util, MEM_util, WB_util = 0;


struct inst {
    OPCODE opcode;
    int rs;
    int rt;
    int rd;
    int immediate;
};

struct latch{
    bool flag;
    int operandOne;
    int operandTwo;
    int destRegister;
    OPCODE opcode;
    int counter;
    struct inst instruction;
};

struct latch IF, IF_ID, ID, ID_EX, EX, EX_MEM, MEM, MEM_WB, WB;

/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char *argv[]){
    int i;
    m = 5;
    n = 5;
    
    registerFile = (int *) malloc(32*sizeof(int));
    for(i=0; i<32; i++){
        registerFile[i] = 0;
    }
    registerFlags = (bool*) malloc(32*sizeof(bool));
    for(i=0; i<32; i++){
        registerFile[i] = true;
    }
    
    ID_EX.flag = true;
    ID_EX.operandOne = 9;
    ID_EX.operandTwo = 44;
    ID_EX.destRegister = 20;
    ID_EX.counter = 0; //ignore this counter
    ID_EX.opcode = BEQ;
    
    EX.flag = false;
    EX.operandOne = 4;
    EX.operandTwo = 3;
    EX.destRegister = 9;
    EX.counter = 0;
    EX.opcode = ADD;
    
    EX_MEM.flag = false;
    EX_MEM.operandOne = 6;
    EX_MEM.operandTwo = 1;
    EX_MEM.destRegister = 8;
    EX_MEM.counter = 0; //ignore this counter
    EX_MEM.opcode = SUB;
    
    
    printf("(ID_EX) flag: %d op1: %d op2: %d rd: %d counter %d opcode: %d\n", ID_EX.flag, ID_EX.operandOne, ID_EX.operandTwo, ID_EX.destRegister, ID_EX.counter, ID_EX.opcode);
    printf("(EX) flag: %d op1: %d op2: %d rd: %d counter: %d opcode: %d\n", EX.flag, EX.operandOne, EX.operandTwo, EX.destRegister, EX.counter, EX.opcode);
    printf("(EX_MEM) flag: %d op1: %d op2: %d rd: %d counter %d opcode: %d\n\n", EX_MEM.flag, EX_MEM.operandOne, EX_MEM.operandTwo, EX_MEM.destRegister, EX_MEM.counter, EX_MEM.opcode);
    printf("utilization: %d\n", EX_util);
    
    
    return 0;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void EX_stage(){
    //if EX stage is in the middle of an operation and the counter for the stage is greater than 1,
    //increment the utilization for EX and decrement the counter so that we know EX was doing work and so
    //the operation advances
    if(EX.counter > 1){
        EX.counter--;
        EX_util++;
    }
    //if EX has completed its operation but the producer latch (ID_EX) doesn't have a fresh instruction,
    //"do nothing" /void operation
    else
        if (EX.counter == 0 && ID_EX.flag == false)
            ;
    //If EX has completed its operation and the producer latch is ready with an instruction, pull in
    //the instruction to an internal latch, and set the flag on the producer latch to false which means it
    //has an old instruction and can be overwritten by the producer, ID.
    
    //Increment the utilization of EX and set the counter to the new counter as determined by m or n.
    //The counter for the stage will be set to one less than m or n, as it has completed one cycle of the new
    //instruction by the end of this function call.
    //The special case of m or n being equal to 1 is dealt with by completing the operation immediately.
    if (EX.counter == 0 && ID_EX.flag == true){
        EX.operandOne=ID_EX.operandOne;
        EX.operandTwo=ID_EX.operandTwo;
        EX.destRegister=ID_EX.destRegister;
        EX.opcode=ID_EX.opcode;
        ID_EX.flag=false;
        //For multiply, set the counter to m-1 for the next function call, but in the case that m=1, do the
        //multiply if the proceding/consumer latch is open. If not, set the EX.counter to 1 and fall through
        //to the normal ALU block on the next call (the block for the condition EX.counter == 1).
        EX.counter = m-1;
        if(EX.counter == 0){
            //Do multiply
            if(EX_MEM.flag == false){
                //transfer result of MUL and instruction details into EX_MEM latch
                //set flag to show that a new instruction is available to MEM
                EX_MEM = EX;
                EX_MEM.operandOne = EX.operandOne*EX.operandTwo;
                EX_MEM.flag = true;
                EX_util++;
            }
            else EX.counter = 1;
        }
    }
    else{
        //For other ALU operations, set the counter to n-1 for the next function call, but in the case
        //that n=1, do the operation if the consumer latch is open. If not, set the EX.counter to 1 and
        //fall through to the normal ALU block on the next call (the block for the condition
        //EX.counter == 1).
        EX.counter = n-1;
        if(EX.counter == 0){
            //Do other arithmetic operation, etc.
            if(EX_MEM.flag == false){
                EX_util++;
                if(EX.opcode == SUB){
                    EX_MEM = EX;
                    EX_MEM.operandOne = EX.operandOne - EX.operandTwo;
                    EX_MEM.flag = true;
                }
                if(EX.opcode == BEQ){
                    if(EX.destRegister != EX.operandTwo)
                        branchFlag = false;
                    else{
                        PC = PC + EX.operandOne*4;
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
            else EX.counter = 1;
            
        }
    }
    
    //When the counter finally gets down to 1, if the EX_MEM latch is open, do the operation and dump the
    //result into the latch. If the consumer latch is not available, the block is skipped, the function
    //returns, and the counter stays at 1 for the next function call.
    if(EX.counter == 1){
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
                    PC = PC + EX.operandOne*4;
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
    
}


