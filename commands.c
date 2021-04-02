
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "nutshell_lib.h"

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

void setalias_cmd(const char* name, const char* word) {
    for (int i = 0; i < aliasIndex; i++) {
		if(strcmp(name, word) == 0) {
			printf("Error, expansion of \"%s\" would create a loop.\n", name);
			return;
		} else if((strcmp(aliasTable.name[i], name) == 0) && (strcmp(aliasTable.word[i], word) == 0)) {
			printf("Error, expansion of \"%s\" would create a loop.\n", name);
			return;
		} else if(strcmp(aliasTable.name[i], name) == 0) {
			strcpy(aliasTable.word[i], word);
			return;;
		}
	}
	strcpy(aliasTable.name[aliasIndex], name);
	strcpy(aliasTable.word[aliasIndex], word);
	aliasIndex++;

    printd("New Alias Name> ", name);
    printd("New Alias Val> ", word);
}

void echo_cmd(const char* val) {
    printf("%s\n", val);
}