%{
    #include <stdlib.h>
	#include <stdio.h>
    #include <string.h>
    #include "nutshell_cmds.h"
    
	int yylex(void);

    void yyerror(char*);

    struct cmd_tbl {
        char* command;
        char* option, option2; 
        char* arguements; 
        char* stdin;
        char* stdout;
        char* stderr;
    };
%}

%union 
{
    char* string;
}

%token CD BYE PWD

%token<string> WORD
%token DOT TILDE
%token UNDEFINED

%type<string> input command

%%

input: command
    ;

command:
      CD WORD           {cd_cmd();} 
    | CD                {cd_home();}  
    | PWD               {pwd_cmd();} 
    | BYE               {bye_cmd();} 
    | UNDEFINED         {printf("\tUndefined Character.\n");}
    ;

%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}