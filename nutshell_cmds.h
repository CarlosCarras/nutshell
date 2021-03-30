
#ifndef NUTSHELL_CMDS_H
#define NUTSHELL_CMDS_H

#include <unistd.h>
#include <errno.h>

#define ENV_HOME getenv("HOME")

struct cmd_tbl {
    char* command;
    char* options; 
    char* arguements; 
    char* standardin;
    char* standardout;
    char* standarderr;
    int background;
};

void printerr();

void cd_home();
void cd_cmd(const char* dest);
void pwd_cmd();
void bye_cmd();
void setenv_cmd(const char* var, const char* val);
void envexp_cmd(const char* var);

struct cmd_tbl buildtable(char* command, 
                          char* options, 
                          char* arguements,  
                          char* standarddin,
                          char* stdandardout,
                          char* stdandarderr,
                          int background
);

#endif // NUTSHELL_CMDS_H