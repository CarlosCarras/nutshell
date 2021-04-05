#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "nutshell_lib.h"

/*************************** Var Table ***************************/

void setVar(const char* name, const char* word) {
    strcpy(varTable.var[varIndex], name);
    strcpy(varTable.word[varIndex], word);
    varIndex++;
}

void setStartupVars() {
    getcwd(cwd, sizeof(cwd));
    
    setVar("HOME", cwd);
    setVar("PATH", ".:/bin");
}

/************************** Alias Table **************************/

char* subAliases(char* name) {
    for (int i = 0; i < aliasIndex; i++) {
        if(strcmp(aliasTable.name[i], name) == 0) {
            return aliasTable.word[i];
        }
    }
    return name;
}

int isAlias(char* name) {
    for (int i = 0; i < aliasIndex; i++) {
        if(strcmp(aliasTable.name[i], name) == 0) {
            return 1;
        }
    }
    return 0;
}

/************************ Print Functions ************************/

void printd(const char* desc, const char* val) {
    #ifdef DEBUG_NUTSHELL
        printf("\tDEBUG: %s %s\n", desc, val);
    #endif // DEBUG_NUTSHELL
}

void printerr() {
    char err_str;

    printf("error: ");

    switch(errno) {
        /* chdir, putenv */
        case EACCES      : printf("search permission is denied for one of the components of path.\n"); break;
        case EFAULT      : printf("path points outside your accessible address space.\n"); break;
        case EIO         : printf("an I/O error occurred.\n"); break;
        case ELOOP       : printf("too many symbolic links were encountered in resolving path.\n"); break;
        case ENAMETOOLONG: printf("path is too long.\n"); break;
        case ENOENT      : printf("the directory specified in path does not exist.\n"); break;
        case ENOMEM      : printf("insufficient kernel memory was available.\n"); break;
        case ENOTDIR     : printf("q component of path is not a directory.\n"); break;
    }
}