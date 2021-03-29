
#ifndef NUTSHELL_CMDS_H
#define NUTSHELL_CMDS_H

#include <unistd.h>
#include <errno.h>

#define ENV_HOME getenv("HOME")

void printerr();

void cd_home();
void cd_cmd(const char* dest);
void pwd_cmd();
void bye_cmd();
void setenv_cmd(const char* var, const char* val);
void envexp_cmd(const char* var);

#endif // NUTSHELL_CMDS_H