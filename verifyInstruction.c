/*
INSTRUCTIONS FOR RUNNING - READ THIS SHIT FIRST OR IT WON'T WORK

-----------------------------------------------------------------
-----------------------------------------------------------------

Compile in terminal the normal way (gcc -oa.out verifyInstruction.c -lm)
Call the program with and instruction, with each argument separated by spaces (NOT commas)
$, (, ), and possibly other characters must be prepended with \
so a typical add instruction would be

./a.out add \$0 \$1 \$2

and a lw/sw would look like

./a.out lw \$0 123\(\$1\)

The reason you have to do this is that terminal uses $, (, and ) as special characters. \ tells it to ignore the under-the-hood meaning of the following char.
The backslashes won't be necessary in the actual code: this just has to do with how terminal reads arguments in, not the actual code itself.
If you enter the input wrong you will either get a segmentation fault or bash will complain. That's not my code doing that, it's the arguments.

Since the code is designed to read in poorly constructed arguments, you could of course input something like

./a.out add \$s0 \$t0 \$t1 \$t2

Which has too many arguments, or

./a.out add a b c

which has invalid register declarations.

What SHOULD happen is that it prints an error message and throws an assert if you make a bad (but properly formatted) input (too many args, bad opcode, bad register/address declare).
If it parses the instruction as valid it'll print "instruction checks out."

-----------------------------------------------------------------
-----------------------------------------------------------------

Some notes about stuff I did which weren't in class:

stdbool.h: Library which allows you to use booleans. They behave exactly how you'd expect (can be used in conditionals, declared as true/false, etc) but C has no native type for it. Declare as

bool b;
b = true;
  
-------------------------

The declaration:

char c;
int i = c-'0';

Takes a character (typically, a character which is an int) and automatically recasts it as an int with the corresponding value. So if c='0', i=0.
Technically what is happening is that I'm subtracting the Unicode/ASCII value of '0' from c as an int. In unicode (int)'0' = 48, (int)'9' = 57, so '9'-'0'=9.
Useful for verifying whether a character (which is expected to be an integer) is within the appropriate bound. If the char isn't an integer it'll fall outside the bound.

-------------------------

int strncmp(char* str1, char* str2, int n)

compares the first n bytes (characters) of str1 and str2. It looks char by char a mismatch, and returns the (integer) value of the mismatch. So, if the first n bytes match,

strncmp(str1, str2, n) = 0

Which is how I used it: I make a bool

(strncmp(str1, str2, n) == 0)

to check for equality of the first n bytes. The bool

(str1=="add")

turns out to be problematic, for some reason, but strncmp seems to always work. Also useful for slicing strings, ex.

(strncmp(str, "$s", 2) == 0)

just verifies that the first two chars in str are "$s."

-------------------------

void sprintf(char* str1, "%d", int n)

Writes n to str1 (NOT concatenation, it's an overrwrite) as a string.

-------------------------

void strcpy(char* str1, char* str2)

copies str2 into str2. Careful about the mallocs

-------------------------

void strcat(char* str1, char* str2)

Concatenates str1 + str2. Be double careful about the mallocs

-------------------------

int strlen(char* str)

Gets the length of str, returns an int.

*/

/////////////////////////////////////////////////////////////////////////////////////////////////////

//Emma Bryce - Eliot Bush - Ygor Jean
//ECE 353 Lab2
//verifyInstruction.c
//10/15/2015

/////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

//globals (so far just functions)
bool verifyInstruction(char **);
bool verifyRegister(char *);
char* translateRegister(char *);
bool verifyAddress(char *);
bool verifyImmediate(char *);

/////////////////////////////////////////////////////////////////////////////////////////////////////

//verifies whether an instruction (token array) is kosher or not
//returns a bool: use it directly in conditionals/asserts
bool verifyInstruction(char **instr){
	//add instruction
	if(strncmp(instr[0], "add", 10)==0){
		//verify arg count
		if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
			printf("bad arg count\n");
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
			printf("bad arg count\n");
			return false;
		}
		else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyRegister(translateRegister(instr[3])));}
	}

	//addi instruction.
	//need some clarification on how to verify immediates, right now it just always returns true if there's something in the imm field
	else if(strncmp(instr[0], "addi", 10)==0){
		if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
			printf("bad arg count\n");
			return false;
		}
		else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyImmediate(instr[3]));}
	}

	//mul instruction
	else if(strncmp(instr[0], "mul", 10)==0){
		if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
			printf("bad arg count\n");
			return false;
		}
		else{return (verifyRegister(translateRegister(instr[1])) && verifyRegister(translateRegister(instr[2])) && verifyRegister(translateRegister(instr[3])));}
	}

	//lw
	else if(strncmp(instr[0], "lw", 10)==0){
		if(instr[3]!=NULL || instr[2]==NULL || instr[1]==NULL){
			printf("bad arg count\n");
			return false;
		}
		//here verifyAddress is used
		else{return (verifyRegister(translateRegister(instr[1])) && verifyAddress(instr[2]));}
	}

	//sw (same as lw)
	else if(strncmp(instr[0], "sw", 10)==0){
		if(instr[3]!=NULL || instr[2]==NULL || instr[1]==NULL){
			printf("bad arg count\n");
			return false;
		}
		else{return (verifyRegister(translateRegister(instr[1])) && verifyAddress(instr[2]));}
	}

	//beq
	else if(strncmp(instr[0], "beq", 10)==0){
		if(instr[4]!=NULL || instr[3]==NULL || instr[2]==NULL || instr[1]==NULL){
			printf("bad arg count\n");
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
bool verifyRegister(char *reg){
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
char* translateRegister(char *reg){
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
bool verifyAddress(char *addr){
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
bool verifyImmediate(char *imm){return true;}

/////////////////////////////////////////////////////////////////////////////////////

//just for testing
int main(int argc, char *argv[]){
	char **test;
	test = (char **) malloc((argc-1)*sizeof(char *));
	int j;
	for(j=1; j<=argc; j++){test[j-1] = argv[j];}

	assert(verifyInstruction(test));
	printf("instruction checks out");
}
