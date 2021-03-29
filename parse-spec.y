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
        int background;
    };
%}

%start input

%union 
{
    char* string;
}

%token CD BYE PWD SETENV

%token<string> WORD
%token NEWLINE INVALID

%%

input: 
    command NEWLINE

command:
      CD                {cd_home(); return 1;}    /* for some reason this path is never taken */
    | CD WORD           {cd_cmd($2); return 1;}  
    | SETENV WORD WORD  {setenv_cmd($2, $3);}
    //| '$' envvar        {envexp_cmd($2);} 
    | PWD               {pwd_cmd(); return 1;} 
    | BYE               {bye_cmd();} 
    | WORD              {printf("error: command not found.\n");}
    | INVALID           {printf("error: invalid arguements.\n");}
    ;

//envvar: '{' WORD '}'    {$$ = $2;}

%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}