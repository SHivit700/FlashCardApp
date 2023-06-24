#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 8
#define MAX_CHARS 32

/*
    Allocates the memory needed to store the result of the tokenizing
*/
char **allocateLines(void) {
    char** lines = (char**)malloc(MAX_WORDS * sizeof(char*)); 
    for (int i = 0; i < MAX_WORDS; i++) {
        lines[i] = (char*)malloc(MAX_CHARS * sizeof(char)); 
    }
    return lines;
}
/*
    Takes in a string to be tokenized, a pointer to memory where the result is stored and the delimiting
    characters that the string is split on.
    The string is split into an array of strings on the delimiting characters.
    Returns the number of tokens created.
*/
int tokenize(char *str, char **res, char *delim) {
    int count = 0;
    char *strCopy = strdup(str);
    
    // Using strtok to tokenize
    char* token = strtok(strCopy, delim);
    while (token != NULL) {
        res[count] = strdup(token);
        count++;
        token = strtok(NULL, delim);
    }
    free(strCopy);
    return count;
}

/*
    Frees the memory allocated for the result
*/
void freeLines(char **lines) {
    //Free the memory allocated for tokens
    for (int i = 0; i < MAX_WORDS; i++) {
        free(lines[i]);
    }
    free(lines);
}