
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "nutshell_cmds.h"

void cd_home() {
    printf("Changed Dir to Home.\n");
}

void cd_cmd() {
    printf("Changed Dir.\n");
}

void pwd_cmd() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("  %s\n", cwd);
}

void bye_cmd() {
    exit(0);
}
