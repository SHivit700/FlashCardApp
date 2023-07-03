#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "readFile.h"
#include "tokenizer.h"
#include "fileIO.h"

questionAnswerLists* initQuestionList(int length) {
    questionAnswerLists *list = (questionAnswerLists *) malloc(sizeof(questionAnswerLists));
    list->size = length;
    list->answers = (char**)malloc(length*sizeof(char*)); 
    list->questions = (char**)malloc(length*sizeof(char*));
    for (int i = 0; i < length; i++) {
        list->answers[i] = malloc(MAX_QUESTION_SIZE*sizeof(char));
        list->questions[i] = malloc(MAX_QUESTION_SIZE*sizeof(char));
    }
    return list;
}

nameScoreLists* initNameList(int length) {
    nameScoreLists *list = (nameScoreLists *) malloc(sizeof(nameScoreLists));
    list->size = length;
    list->names = (char**)malloc(length*sizeof(char*)); 
    list->scores = (int*)malloc(length*sizeof(int*));
    for (int i = 0; i < length; i++) {
        list->names[i] = malloc(MAX_NAME_SIZE*sizeof(char));
    }
    return list;
}

/*
    pass in the file name - creates a struct where
    the first string array will be populated with questions
    the second string array will be populated with answers
    the size is the number of lines
*/
questionAnswerLists* questionRead(char *fileName) {
    int numLines;
    char **lines = readFileLines(fileName, &numLines);
    questionAnswerLists *lists = initQuestionList(numLines);
    for (int i = 0; i < numLines; i++) {
        char *line = lines[i];
        char **splitLine = allocateLines();
        tokenize(line, splitLine, ":");
        strcpy(lists->questions[i], splitLine[0]);
        strcpy(lists->answers[i], splitLine[1]);
        freeLines(splitLine);
    }
    return lists;
}

// converts a char * to an integer
// test edge case ... not a number
int stringToInt(char *str, int len) {
    int res = 0;
    for(int i = 0; i < len; i++) {
        switch(str[i]) {
            case '0':
                res = res*10 + 0;
                break;

            case '1':
                res = res*10 + 1;
                break;

            case '2':
                res = res*10 + 2;
                break;

            case '3':
                res = res*10 + 3;
                break;

            case '4':
                res = res*10 + 4;
                break;

            case '5':
                res = res*10 + 5;
                break;

            case '6':
                res = res*10 + 6;
                break;

            case '7':
                res = res*10 + 7;
                break;

            case '8':
                res = res*10 + 8;
                break;

            case '9':
                res = res*10 + 9;
                break;
        }
    }
    return res;
}

/*
    pass in the file name - creates a struct where
    the first string array will be populated with username
    the second string array will be populated with scores
    the size is the number of lines
*/
nameScoreLists* leaderboardRead(char *fileName) {
    int numLines;
    char **lines = readFileLines(fileName, &numLines);
    nameScoreLists *lists = initNameList(numLines);
    for (int i = 0; i < numLines; i++) {
        char *line = lines[i];
        char **splitLine = allocateLines();
        tokenize(line, splitLine, ":");
        strcpy(lists->names[i], splitLine[0]);
        lists->scores[i] = stringToInt(splitLine[1], strlen(splitLine[1]));

        freeLines(splitLine);
    }
    return lists;
}




/*
    takes in the name of the file to output to and two strings
    that are to be added to the file, separated by a colon
*/
void fileWrite(char *fileName, char *firstString, char *secondString) {
    // only update if score is higher 
    // check if name is empty

    // check if leaderboard
    if(strcmp(fileName, "leaderboard.txt") == 0) {
        nameScoreLists* lists = leaderboardRead(fileName);

        // Find index of the given username 
        int index = -1;
        for (int i = 0; i < lists->size; i++) {
            if (strcmp(lists->names[i], firstString) == 0) {
                index = i;
                break;
            }
        }

        // If the username is found, remove it from the list
        if (index != -1) {
            if(lists->scores[index] > stringToInt(secondString, strlen(secondString))) {
                // do nothing as highscore > currentScore
            } else {
                // Shift the remaining elements to fill the gap
                for (int i = index; i < lists->size - 1; i++) {
                    strcpy(lists->names[i], lists->names[i + 1]);
                    lists->scores[i] = lists->scores[i + 1];
                }
            
                // add the new pair of username:score 
                // make sure only highest score is added

                FILE *out = fopen(fileName, "w");
                if (out == NULL) {
                    printf("Failed to open file\n"); 
                    return;
                }

                for (int i = 0; i < lists->size - 1; i++) {
                    fprintf(out, "%s:%d\n", lists->names[i], lists->scores[i]);
                }

                fprintf(out, "%s:%s\n", firstString, secondString);

                fclose(out);
            }
        } else {
            FILE *out;
            struct stat stat_record;
            if(stat(fileName, &stat_record))
                //if file doesn't exist, create a new one
                out = fopen(fileName, "w");
            else {
                //if file is present, append to it
                out = fopen(fileName, "a");
            }

            if (out == NULL) {
                perror("Failed to open file\n"); 
                return;
            }

            fprintf(out, "%s:%s\n", firstString, secondString);
            fclose(out); 
        }
    } else {
        FILE *out;
        struct stat stat_record;
        if(stat(fileName, &stat_record))
            //if file doesn't exist, create a new one
            out = fopen(fileName, "w");
        else {
            //if file is present, append to it
            out = fopen(fileName, "a");
        }
        if (out == NULL) {
            perror("Failed to open file\n"); 
            return;
        }

        fprintf(out, "%s:%s\n", firstString, secondString);
        fclose(out); 
    }
}

/*
    pass in the file name - creates a struct where
    the string array will be populated with names
    the int array will be populated with scores
    the size is the number of lines
*/
nameScoreLists* scoreRead(char* fileName) {
    int numLines;
    char **lines = readFileLines(fileName, &numLines);
    nameScoreLists *lists = initNameList(numLines);
    for (int i = 0; i < numLines; i++) {
        char *line = lines[i];
        char **splitLine = allocateLines();
        tokenize(line, splitLine, ":");
        strcpy(lists->names[i], splitLine[0]);
        lists->scores[i] = atoi(splitLine[1]);
        freeLines(splitLine);
    }
    
    return lists;
}

void swapScores(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void swapNames(char** a, char** b) {
    char *temp = strdup(*a);
    *a = strdup(*b);
    *b = strdup(temp);
}

int partition(int* scores, char** names, int low, int high) {
    int pivot = scores[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (scores[j] > pivot) {
            i++;
            swapScores(&scores[i], &scores[j]);
            swapNames(&names[i], &names[j]);
        }
    }
    swapScores(&scores[i + 1], &scores[high]);
    swapNames(&names[i + 1], &names[high]);
    return (i + 1);
}

void quickSort(int* scores, char** names, int low, int high) {
    if (low < high) {
        int pi = partition(scores, names, low, high);

        quickSort(scores, names, low, pi - 1);
        quickSort(scores, names, pi + 1, high);
    }
}

/*
    sorts the list of scores and names
*/
void sortScores(nameScoreLists* list){
    quickSort(list->scores, list->names, 0, list->size - 1);
}

/*
    gets the list of top 5 scores
*/
nameScoreLists* getTopScores(const char* fileName) {
    nameScoreLists *lists = scoreRead(fileName);
    sortScores(lists);
    lists->size = NUM_SCORES;
    return lists;
}

void deleteQuestionAnswerPair(const char* fileName, const char* question) {
    questionAnswerLists* lists = questionRead(fileName);

    // Find index of the given question 
    int index = -1;
    for (int i = 0; i < lists->size; i++) {
        if (strcmp(lists->questions[i], question) == 0) {
            index = i;
            break;
        }
    }

    // If the question is found, remove it from the list
    if (index != -1) {
        // Shift the remaining elements to fill the gap
        for (int i = index; i < lists->size - 1; i++) {
            strcpy(lists->questions[i], lists->questions[i + 1]);
            strcpy(lists->answers[i], lists->answers[i + 1]);
        }

        // Decrease the size of the list
        lists->size--;

        // Rewrite the updated question-answer pairs to the file
        FILE* out = fopen(fileName, "w");
        if (out == NULL) {
            perror("Failed to open file\n"); 
            return;
        }

        for (int i = 0; i < lists->size; i++) {
            fprintf(out, "%s:%s\n", lists->questions[i], lists->answers[i]);
        }

        fclose(out);
    }
}

//TESTING

// int main(void) {
//     //write
//     char *fileName = "questions.txt";
//     const int MAX_SCORE_SIZE = 5;
//     char *question = "Hello?";
//     // deleteQuestionAnswerPair(fileName, question);
//     // int score = 50;
//     // char scoreString[MAX_SCORE_SIZE];
//     // sprintf(scoreString, "%d", score);
//     // fileWrite(fileName, name, scoreString);
//     //
//     //read
//     // nameScoreLists *lists = getTopScores(fileName);
//     // for (int i = 0; i < lists->size; i++) {
//     //     printf("name: %s\n", lists->names[i]);
//     //     printf("score: %d\n", lists->scores[i]);
//     // }
//     //
//     // char *fileName2 = "questions.txt";
//     // char *question = "what is 2 + 2";
//     // char *answer = "4";
//     // fileWrite(fileName2, question, answer);
    
//     // questionAnswerLists *lists2 = questionRead(fileName2);
//     // printf("question: %s\nanswer: %s\n", lists2->questions[0], lists2->answers[0]);
//     return 0;
// }


