#ifndef NUTSHELL_LIB_H
#define NUTSHELL_LIB_H

#include <climits>
#include <string>
#include <vector>

/**************************** Defines ****************************/
#define DEBUG_NUTSHELL

#define MAX_WORD_LEN 256
#define MAX_ARGLIST_LEN 1024

#define BLUE   "\033[0;34m"
#define GREEN  "\033[0;32m"
#define CYAN   "\033[0;36m"
#define WHITE  "\033[0;37m"
#define PURPLE "\033[0;35m"
#define RESET  "\x1B[0m"

/**************************** Defines ****************************/

/**************************** Data Types *************************/
typedef struct evTable {
    std::vector<std::string> var;
    std::vector<std::string> word;
} evTable_t;

typedef struct aTable {
    std::vector<std::string> name;
    std::vector<std::string> word;
} aTable_t;

typedef struct keyValPair {
    std::string key;
    std::string val;
} keyValPair_t;

typedef struct cmdTable {
    const char* command;
    const char* options; 
    const char* args; 
    const char* standardin;
    const char* standardout;
    const char* standarderr;
    int background;
} cmdTable_t;
/**************************** Data Types *************************/

/************************ Global Variables ***********************/
extern evTable_t varTable;
extern aTable_t aliasTable;

extern char cwd[PATH_MAX];
extern char args[MAX_ARGLIST_LEN];
/************************ Global Variables ***********************/

/******************** Global Functions ***************************/
void run_cmd(const char* cmd);

void restart();
void addToArglist(const char* word);

bool existsInTable(const std::vector<std::string>& table, const char* key);
bool existsInTable(const std::vector<std::string>& table, const std::string& key);
size_t getTableIndex(const std::vector<std::string>& table, const char* key);
size_t getTableIndex(const std::vector<std::string>& table, const std::string& key);
void removeTableIndex(std::vector<std::string>& table, size_t index);
void setVar(char* name, char* word);
void setStartupVars();
char* subVar(char* var);
int isVar(char* var);
void setAlias(char* name, char* word);
char* subAlias(char* name);
int isAlias(char* name);
int isPattern(char* word);
char* subPattern(char* word);
void printd(const char* desc, const char* val);
void printd(const std::string& desc, const std::string& val);
void printerr();
/*************************** Functions ***************************/

#endif // NUTSHELL_LIB_H