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
    printf("%s\n",argv[1]);
    char imm = *(argv[1]);
    if(imm=='g'){printf("word\n");}
    else(printf("still grinding");
    return 0;
}


