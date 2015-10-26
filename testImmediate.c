//
//  testImmediate.c
//  
//
//  Created by Ygorsunny Jean on 10/25/15.
//
//

#include <stdio.h>

/*
bool verifyImmediate(char *);

bool verifyImmediate(char *imm){
    
    if((imm[0]-'0')<0 || (imm[0]-'0')>9){printf("not immediate: %s", imm); return false;}
    
    
    int i;
    //while pointer is pointing to a valid integer..
    while(imm[i]-'0'>=0){
        char c = *imm;
        int num = c-'0';
        i++;
    }
    

    
    return true;}
*/

int main(int argc, const char * argv[]) {
    int i;
    int j;
    const char * strChar;
    printf("%d\n",argc);
    for(i=1;i<argc;i++){
        printf("%s\n",argv[i]);
        strChar = argv[i];
        for(j = 0; j<strlen(strChar); j++){
            printf("%c\n",strChar[0]);//this line is a char in the string array!
        }
        
    }

    
    
    
    
    /*char imm = *(argv[1]);
    if(imm=='g'){printf("Word\n");}
    else(printf("Still grinding\n"));
    if((imm -'0')<0 || (imm -'0'>9)){printf("Not valid\n");}
    else(printf("Valid\n"));
    */
    return 0;

}


