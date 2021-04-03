
#ifndef NUTSHELL_CMDS_H
#define NUTSHELL_CMDS_H

#include <unistd.h>
#include "nutshell_lib.h"

#define ENV_HOME getenv("HOME")

void cd_home();
void cd_cmd(const char* dest);
void pwd_cmd();
void echo_cmd(const char* val);
void bye_cmd();

/********************* Environment Variables *********************/
void envexp_cmd(const char* var);
void setenv_cmd(const char* var, const char* val);
void unsetenv_cmd(const char* var);
void printenv_cmd();

/***************************** Alias *****************************/
void setalias_cmd(const char* name, const char* val);
void unalias_cmd(const char* name);
void printalias_cmd();

/************************* Other Command *************************/
void handle_cmd(const char* command, 
                const char* options, 
                const char* args,  
                const char* standarddin,
                const char* stdandardout,
                const char* stdandarderr,
                int background
);

void interpret_cmd(struct cmdTable *cmd);

#endif // NUTSHELL_CMDS_H