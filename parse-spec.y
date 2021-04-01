%{
    #include <stdlib.h>
	#include <stdio.h>
    #include <string.h>
    #include "commands.h"
    #include "nutshell_lib.h"
    
	int yylex(void);

    void yyerror(char*);
%}

%start input

%union 
{
    char* string;
}

%token CD BYE PWD SETENV ALIAS ECHO_CMD

%token<string> WORD
%token NEWLINE INVALID

%%

input: 
    line NEWLINE

line: CD                {cd_home();}
    | CD WORD           {cd_cmd($2);} 
    | SETENV WORD WORD  {setenv_cmd($2, $3);}
    | PWD               {pwd_cmd();} 
    | BYE               {bye_cmd();} 
    | ALIAS WORD WORD   {setalias_cmd($2, $3);}
    | ECHO_CMD WORD     {echo_cmd($2);}
    | WORD              {buildTable($1, "\0", "\0", "\0", "\0", "\0", 0);}
    | INVALID           {printf("error: invalid arguements.\n");}
    ;

%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}