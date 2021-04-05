
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

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
    printf("%s\n", cwd);
}

void echo_cmd(const char* val) {
    printf("%s\n", val);
}

void bye_cmd() {
    exit(0);
}

/********************* Environment Variables *********************/

void envexp_cmd(const char* var) {
    char* val = getenv(var);
    printf("%s\n", var);
}

void setenv_cmd(const char* var, const char* word) {
    for (int i = 0; i < varIndex; i++) {
		if(strcmp(var, word) == 0) {
            /* if variable == word */
			printf("error: setting variable \"%s\" would create a loop.\n", var);
			return;
		} else if((strcmp(varTable.var[i], var) == 0) && (strcmp(varTable.word[i], word) == 0)) {
		    /* if both the variable and the word already exist */
			return;
		} else if(strcmp(varTable.var[i], var) == 0) {
            /* redefine the variable if variable exists */
			strcpy(varTable.word[i], word);
			return;
		}
	}
    /* else the variable is new, and must be added to the table */
	strcpy(varTable.var[varIndex], var);
	strcpy(varTable.word[varIndex], word);
	varIndex++;

    printd("New Variable Name> ", var);
    printd("New Variable Val> ", word);
}

void unsetenv_cmd(const char* var) {
    for (int i = 0; i < varIndex; i++) {
        if (strcmp(varTable.var[i], var) == 0) {    // if the variable exists in the table...
            for (int j = i; j < varIndex-1; j++) {  // ...remove entry by compressing array
                strcpy(varTable.var[j], varTable.var[j+1]);
                strcpy(varTable.word[j], varTable.word[j+1]);
            }
            varIndex--;
        }
    }
}

void printenv_cmd() {
    for (int i = 0; i < varIndex; i++) {
        printf("%s=%s\n", varTable.var[i], varTable.word[i]);
    }
}

/***************************** Alias *****************************/

void setalias_cmd(const char* name, const char* word) {
    for (int i = 0; i < aliasIndex; i++) {
		if(strcmp(name, word) == 0) {
            /* if alias == word */
			printf("error: expansion of \"%s\" would create a loop.\n", name);
			return;
		} else if((strcmp(aliasTable.name[i], name) == 0) && (strcmp(aliasTable.word[i], word) == 0)) {
			/* if both the alias and the word already exist */
			return;
		} else if(strcmp(aliasTable.name[i], name) == 0) {
            /* redefine the alias if alias exists */
			strcpy(aliasTable.word[i], word);
			return;
		}
	}
    /* else the alias is new, and must be added to the table */
	strcpy(aliasTable.name[aliasIndex], name);
	strcpy(aliasTable.word[aliasIndex], word);
	aliasIndex++;

    printd("New Alias Name> ", name);
    printd("New Alias Val> ", word);
}

void unalias_cmd(const char* name) {
    for (int i = 0; i < aliasIndex; i++) {
        if (strcmp(aliasTable.name[i], name) == 0) {    // if the alias exists in the table...
            for (int j = i; j < aliasIndex-1; j++) {    // ...remove entry by compressing array
                strcpy(aliasTable.name[j], aliasTable.name[j+1]);
                strcpy(aliasTable.word[j], aliasTable.word[j+1]);
            }
            aliasIndex--;
        }
    }
}

void printalias_cmd() {
    for (int i = 0; i < aliasIndex; i++) {
        printf("%s=%s\n", aliasTable.name[i], aliasTable.word[i]);
    }
}

/************************* Other Command *************************/

void handle_cmd(const char* command, 
                const char* options, 
                const char* args,  
                const char* standarddin,
                const char* stdandardout,
                const char* stdandarderr,
                int background
) {
    struct cmdTable cmd;

    cmd.command = command;
    cmd.options = options;
    cmd.args = args;
    cmd.standardin = standarddin;
    cmd.standardout = stdandardout;
    cmd.standarderr = stdandarderr;
    cmd.background = background;

    interpret_cmd(&cmd);
}

void interpret_cmd(struct cmdTable *cmd) {
    if (cmd) {
        system(cmd->command);
        printd("CMD:", cmd->command);
        printd("ARGS:", cmd->args);
    }
}