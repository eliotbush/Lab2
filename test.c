#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, const char * argv[]) {
    char ***instructionMemory;
    char *line;
    char delimiters[] = ", ";
    int i;
    int j;
    int k;
    instructionMemory = (char***) malloc(512*sizeof(char**));
    for(i=0; i<512; i++){
        instructionMemory[i] = (char**) malloc(10*sizeof(char*));
    }    


    FILE *assembly_program = fopen("assemblyProgram.txt", "rt");
    assert (assembly_program != NULL);
    char *token;

    j=0;
    while (fgets(line, 100, assembly_program) != NULL){
	i=0;
    	token = strtok(line, delimiters);
        while(token != NULL){
            instructionMemory[j][i] = token;
            token = strtok(NULL, delimiters);
            i++;
        }
        printf("%d: %s %s %s %s\n", j, instructionMemory[j][0], instructionMemory[j][1], instructionMemory[j][2], instructionMemory[j][3]);
        //printf("%d: %s %s %s %s\n", j, p[0], p[1], p[2], p[3]);
        //memcpy(*(instructionMemory+j), p, 100);
	j++;
    }

    for(i=0;i<j;i++){printf("%s %s %s %s\n", instructionMemory[i][0], instructionMemory[i][1], instructionMemory[i][2], instructionMemory[i][3]);}
        
    

}
