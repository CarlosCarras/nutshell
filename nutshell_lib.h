#include <limits.h>

#define DEBUG 1

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

struct aTable aliasTable;
int aliasIndex, varIndex;
char cwd[PATH_MAX];

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