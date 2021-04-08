#ifndef NUTSHELL_LIB_H
#define NUTSHELL_LIB_H 

#include <limits.h>

#define DEBUG_NUTSHELL

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
    const char* command;
    const char* options; 
    const char* args; 
    const char* standardin;
    const char* standardout;
    const char* standarderr;
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
char* subVar(char* var);
int isVar(char* var);
char* subAlias(char* name);
int isAlias(char* name);
void printd(const char* desc, const char* val);
void printerr();

#endif // NUTSHELL_LIB_H
