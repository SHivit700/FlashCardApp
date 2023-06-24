#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "readFile.h"

/*
    Takes in a file name and a pointer to size.
    The file is read from and the data is stored as an array of lines which are returned.
    The value of size is set to the number of lines read.
*/
char** readFileLines(char* fileName, int* size) {
    FILE *file = fopen(fileName, "r");
   
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return NULL; 
    }

    int currentChar; 
    int lineCount = 0;

    //gets the number of lines in the file
    while ((currentChar = fgetc(file)) != EOF) {
        if (currentChar == '\n') {
            lineCount++;
        }
    }

    fseek(file, 0, SEEK_SET);
    
    char** lines = (char**) malloc(lineCount * sizeof(char*));

    if (lines == NULL) {
        perror("Failed to allocate memory");
    }


    size_t bufferSize = 0;
    for (int i = 0; i < lineCount; i++) {
        lines[i] = (char*) malloc(sizeof(char) * MAX_LINE_SIZE);
        if (getline(&lines[i], &bufferSize , file) == -1) {
            free(lines[i]);
            //Either failure or EOF
            fclose(file);
            for (int j = 0; j < i; j++) {
                free(lines[j]);  // Free memory for previous lines
            }
            free(lines);  // Free memory for the array of lines
            return NULL;
        }
        char *ptr = strchr(lines[i], '\n');
        if (lines[i] != NULL) {
            *ptr = '\0';
        }
    }
    
    fclose(file);
    *size = lineCount;
    return lines; 
}
