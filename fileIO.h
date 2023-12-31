#define MAX_NAME_SIZE 32
#define MAX_QUESTION_SIZE 1024
#define NUM_SCORES 5
#define SCORE_FILE "leaderboard.txt"
#define QUESTION_FILE "questions.txt"

typedef struct nameScoreLists 
{
    char **names;
    int *scores;
    int size;
} nameScoreLists;

typedef struct questionAnswerLists 
{
    char **questions;
    char **answers;
    int size;
} questionAnswerLists;

extern int stringToInt(char *str, int len);
extern questionAnswerLists* initQuestionList(int length);
extern nameScoreLists* initNameList(int length);
extern nameScoreLists* leaderboardRead(char *fileName);
extern void fileWrite(char *fileName, char *firstString, char *secondString);
extern nameScoreLists* scoreRead(char* fileName);
extern questionAnswerLists* questionRead(char *fileName);
extern nameScoreLists* getTopScores(const char* fileName);
extern void deleteQuestionAnswerPair(const char* fileName, const char* question);
