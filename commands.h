#ifndef NUTSHELL_CMDS_H
#define NUTSHELL_CMDS_H

extern "C" {
#include <unistd.h>
}
#include "nutshell_lib.h"

void cd_home();
void cd_cmd(char* dest);
void pwd_cmd();
void echo_cmd(char* val);
void bye_cmd();

/********************* Environment Variables *********************/
void envexp_cmd(char* var);
void setenv_cmd(char* var, char* val);
void unsetenv_cmd(char* var);
void printenv_cmd();

/***************************** Alias *****************************/
void setalias_cmd(char* name, char* val);
void unalias_cmd(char* name);
void printalias_cmd();

/************************* Other Command *************************/
void unknown_command();

void handle_cmd(const char* command, 
                const char* options, 
                const char* args,  
                const char* standardin,
                const char* stdandardout,
                const char* stdandarderr,
                int background
);

void interpret_cmd(const cmdTable_t& cmd);

#endif // NUTSHELL_CMDS_H