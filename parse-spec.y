%{
    #include <stdlib.h>
	#include <stdio.h>
    #include <string.h>
    #include "nutshell_cmds.h"
    
	int yylex(void);

    void yyerror(char*);
%}

%union 
{
    char* string;
}

%token CD BYE PWD

%token<string> WORD
%token DOT TILDE
%token UNDEFINED

%type<string> command

%%

command: /* empty */    
    | CD WORD WORD      {cd_cmd();} 
    | CD                {cd_home();}  
    | PWD               {pwd_cmd();} 
    | BYE               {bye_cmd();} 
    | UNDEFINED         {printf("\tUndefined Character.\n");}
    ;

%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}