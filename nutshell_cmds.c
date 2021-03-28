
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nutshell_cmds.h"

void cd_home() {
    char* dest = getenv("HOME");
    cd_cmd(dest);
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

void printerr() {
    char err_str;

    printf("error: ");

    switch(errno) {
        /* chdir */
        case EACCES      : printf("Search permission is denied for one of the components of path.\n"); break;
        case EFAULT      : printf("Path points outside your accessible address space.\n"); break;
        case EIO         : printf("An I/O error occurred.\n"); break;
        case ELOOP       : printf("Too many symbolic links were encountered in resolving path.\n"); break;
        case ENAMETOOLONG: printf("path is too long.\n"); break;
        case ENOENT      : printf("The directory specified in path does not exist.\n"); break;
        case ENOMEM      : printf("Insufficient kernel memory was available.\n"); break;
        case ENOTDIR     : printf("A component of path is not a directory.\n"); break;
    }
}