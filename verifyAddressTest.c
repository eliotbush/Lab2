#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
///////////////////////////////////////////////////////////////////////////////////////////////////
bool verifyInstruction(char **);
bool verifyRegister(char *);
char* translateRegister(char *);
bool verifyAddress(char *);
bool verifyImmediate(char *);
struct inst convertInstruction(char **);
/////////////////////////////////////////////////////////////////////////////////////////////////////
//verify a NUMERICAL valued register.
bool verifyRegister(char *Reg){
    char *c;
printf("error\n");
    c = "\n";
    char *reg;
reg = (char *) malloc(10*sizeof(char));
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
/////////////////////////////////////////
bool verifyAddress(char *Addr){
bool isValid;
char * offset;
char * reg;
int i;
int j;
int k = 0;
int y;
printf("%s\n",Addr);
if((Addr[0] - '0')>9 || Addr[0]- '0'<0){
    return false;
}
if(Addr[(strlen(Addr)-1)]!=')'){
    return false;
}
for(i = strlen(Addr)-1; i>-1;i--){
    printf("loop %d: %c\n", i+1, Addr[i]);
        if(Addr[i] == '$'){
            reg = (char *) malloc(10*sizeof(char));
            for(j=i; j<strlen(Addr)-1; j++){
                reg[k]=Addr[i+k];
                printf("nested loop %d: %c\n",k, Addr[i+k]);
                k++;
            }
        
        }
        
        


}
printf("%s\n", reg);
   return true;
}
   /* //if the first char isn't a number quit out
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
    }*/

/////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, const char* argv[]){
char * test;
test = (char *) malloc(10*sizeof(char));
test = "100($s1)";
printf("Main\n");
if(verifyAddress(test)){
printf("Addressed Verified\n");
}
else if(!verifyAddress(test)){
    printf("Invalid Address\n");
}

return 0;
}

