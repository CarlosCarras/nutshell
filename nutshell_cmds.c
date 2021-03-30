
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nutshell_cmds.h"

void cd_home() {
    cd_cmd(ENV_HOME);
}

void cd_cmd(const char* dest) {
    int status = chdir(dest);
    if (status < 0) printerr();
}

void pwd_cmd() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("  %s\n", cwd);
}

void bye_cmd() {
    exit(0);
}

void setenv_cmd(const char* var, const char* val) {
    int status;
    char str[128];

    strcpy(str, var);
    strcat(str, "=");
    strcat(str, val);

    status = putenv(str);
    if (status < 0) printerr();
}

void envexp_cmd(const char* var) {
    char* val = getenv(var);
    printf("%s\n", var);
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

/************************* Data Handling ***********************/

struct cmd_tbl buildtable(char* command, 
                          char* options, 
                          char* arguements,  
                          char* standarddin,
                          char* stdandardout,
                          char* stdandarderr,
                          int background
) {
    struct cmd_tbl cmd;

    cmd.command = command;
    cmd.options = options;
    cmd.arguements = arguements;
    cmd.standardin = standarddin;
    cmd.standardout = stdandardout;
    cmd.standarderr = stdandarderr;
    cmd.background = background;

    return cmd;
}