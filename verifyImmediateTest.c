
/*///////////////////////////////
1.Main is included
2.To test an input
3.Call ./a.out with desired test input

Examples:
./a.out 12
./a.out 09
./a.out b3

///////////////////////////////*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

///////////////////////////////


///////////////////////////////

bool verifyImmediate(char *imm){
int i;
char * neg;
neg = (char *) malloc(((strlen(imm))-1)*sizeof(char));
int value;
bool isNeg = false;
bool isNumber = true;
bool isBit = false;
bool isValid = false;
bool isWTF = false;
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

if((imm[0] - '0') == 0){
        isNumber = true;
        for (i = 1; i<strlen(imm); i++){
                if((imm[i] - '0')>9 || (imm[i] - '0')<0){
                        isNumber = false;
}
}
}
//if it is a negative number than put the value into a pointer and convert that to an int
if(isNeg&&isNumber){
for(i = 1; i<strlen(imm); i++){
        neg[i-1] = imm[i];
//      printf("%c:%c\n",imm[i],neg[i-1]);
}
        value = atoi(neg);
        printf("Value(-) = %d\n", value);
        for (i = 0; i<strlen(imm); i++){
//              printf( "%c\n",neg[i]);
                }

}
//otherwise convert from the input pointer
if(!isNeg&&isNumber){
value = atoi(imm);
printf("Value = %d\n", value);
}
if(value < 0){isBit = false;
        isWTF = true;
        printf("WTF!\n");}
        //isBit Test goes here
if(isNumber){
if(32768-value > 0){
        isBit = true;
}
}

if (isNumber && isBit && !isWTF){
        isValid = true;
}
return isValid;
}

/////////////////////////////

int main(int argc, const char* argv[]){
printf("Main\n");
char * test;
test = (char *) malloc(strlen(argv[1])*sizeof(char));
test = (char *) argv[1];
if(verifyImmediate(test)){
        printf("Valid\n");
}
if(!verifyImmediate(test)){
        printf("Invalid\n");
}
return 0;
}


