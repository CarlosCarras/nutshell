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

%token CD BYE PWD SETENV UNSETENV PRINTENV ALIAS UNALIAS PRINTALIAS INVALIDALIAS ECHO_CMD

%token<string> WORD
%token END INVALID

%%

input:
      line END          {return 1;}
    ;

line: CD                {cd_home();}
    | CD WORD           {cd_cmd($2);} 
    | PWD               {pwd_cmd();} 
    | ECHO_CMD WORD     {echo_cmd($2);}
    | BYE               {bye_cmd();} 
    | SETENV WORD WORD  {setenv_cmd($2, $3);}
    | UNSETENV WORD     {unsetenv_cmd($2);}
    | PRINTENV          {printenv_cmd();}
    | ALIAS WORD WORD   {setalias_cmd($2, $3);}
    | UNALIAS WORD      {unalias_cmd($2);}
    | PRINTALIAS        {printalias_cmd();}
    | INVALID           {printf("error: invalid arguements.\n");}
    | INVALIDALIAS      {printf("error: invalid alias name.\n");}
    |                   {printf("error: unknown command.\n"); return 1;}
    ;

%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}