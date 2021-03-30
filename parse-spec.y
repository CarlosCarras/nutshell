%{
    #include <stdlib.h>
	#include <stdio.h>
    #include <string.h>
    #include "nutshell_cmds.h"
    
	int yylex(void);

    void yyerror(char*);
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
    line NEWLINE

line:
    WORD                {buildtable($1, "\0", "\0", "\0", "\0", "\0", 0); return 1;}
    ;
    
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