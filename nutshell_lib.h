#include <limits.h>

#define DEBUG_NUTSHELL 1

/**************************** Structs ****************************/
struct evTable {
   char var[128][100];
   char word[128][100];
};

struct aTable {
	char name[128][100];
	char word[128][100];
};

struct cmdTable {
    char* command;
    char* options; 
    char* arguements; 
    char* standardin;
    char* standardout;
    char* standarderr;
    int background;
};

/************************ Global Variables ***********************/
struct evTable varTable;
struct aTable aliasTable;

int aliasIndex, varIndex;
char cwd[PATH_MAX];

/*************************** Functions ***************************/
void setVar(const char* name, const char* word);
void setStartupVars();
char* subAliases(char* name);
int isAlias(char* name);
void printd(const char* desc, const char* val);
void printerr();
struct cmdTable buildTable(char* command, 
                           char* options, 
                           char* arguements,  
                           char* standarddin,
                           char* stdandardout,
                           char* stdandarderr,
                           int background
);