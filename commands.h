
#ifndef NUTSHELL_CMDS_H
#define NUTSHELL_CMDS_H

#include <unistd.h>

#define ENV_HOME getenv("HOME")

void cd_home();
void cd_cmd(const char* dest);
void pwd_cmd();
void bye_cmd();
void setenv_cmd(const char* var, const char* val);
void envexp_cmd(const char* var);
void setalias_cmd(const char* name, const char* val);
void echo_cmd(const char* val);

#endif // NUTSHELL_CMDS_H