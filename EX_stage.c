
//Here's the EX stage not officially debugged, but probably right, comments not done
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
void MEM(void);
void IF(void);
void ID(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {ADD, SUB, ADDI, MUL, LW, SW, BEQ} OPCODE;

struct latch {
    bool flag;
    int operandOne;
    int operandTwo;
    int destRegister;
    int counter;
    OPCODE opcode;
};

struct inst {
    OPCODE opcode;
    int rs;
    int rt;
    int rd;
    int immediate;
};

bool branchFlag;
int pc;
int c;
int IFutilization;
int IDutilization;
int MEMutilization;
struct latch IFIDlatch;
struct latch IDEXlatch;
struct latch EXMEMlatch;
struct latch MEMWBlatch;
struct latch IFlatch;
struct latch IDlatch;
struct latch MEMlatch;
int *dataMemory;
int *registers;
bool *registerFlags;
struct inst *instructionMemory;
 

void EX_stage(){
    //if EX stage is in the middle of an operation and the counter for the stage is greater than 1,
    //increment the utilization for EX and decrement the counter so that we know EX was doing work and so the
    //the operation advances
    if(EX.counter > 1){
        EX.counter--;
        EX_util++;
    }
    //if EX has completed its operation but the producer latch (ID_EX) doesn't have a fresh instruction,
    //"do nothing" void operation
    else
        if (EX.counter == 0 && ID_EX.flag == false);
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
        if(EX.opcode ==  MUL){
            EX.counter = m-1;
            if(EX.counter == 0){
                //Do multiply
                if(EX_MEM.flag == false){
                    //order of instructions important
                    EX_MEM = EX;
                    EX_MEM.operandOne = EX.operandOne*EX.operandTwo;
                    EX_MEM.flag = true;
                    EX_util++;
                }
                else EX.counter = 1;
            }
        }
        else{
            EX.counter = n-1;
            if(EX.counter == 0){
                //Do other arithmetic operation
                if(EX_MEM.flag == false){
                    EX_util++;
                    if(EX.opcode == SUB){
                        //order of instructions important
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
                else EX.counter = 1;

            }
        }
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
                    //order of instructions important
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
        }
        
    }
    
}
