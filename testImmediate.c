##include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>



int main(int argc, const char * argv[]) {
    int i;
    int j;
    int value;
    const char * strChar;
    const char * neg;
    //printf("%d\n",argc);
    for(i=1;i<argc;i++){
        printf("%s\n",argv[i]);
        strChar = argv[i];
        value = atoi(strChar);
        if (strChar[0] == '-'){
        	printf("This is a negative value/n");
        
        }
        for(j = 0; j<strlen(strChar); j++){
            printf("%c\n",strChar[j]);//this line is a char in the string array!
        }
        
    }
    return 0;
    }
