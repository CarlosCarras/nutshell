%{
    #include <cstdlib>
    #include <cstdio>
    #include <cstring>
    #include <string>
    #include <iostream>
    #include "commands.h"
    #include "nutshell_lib.h"
    
    int yylex(void);
    int yyerror(char*);

    extern int yylineno;
%}

%start input

%union 
{
    char* string;
    int boolean;
}

%token CD BYE PWD SETENV UNSETENV PRINTENV ALIAS UNALIAS INVALIDALIAS ECHO_CMD
%token STDIN STDOUT1 STDOUT2 STDERRF STDERRO
%token END INVALID FILEEND

%token<string> WORD

%type<string> cmd arglist inputfile outputfile1 outputfile2 errfile
%type<boolean> background

%%

input: command END         {return 1;}

command: FILEEND               {bye_cmd();}
       | CD                {cd_home();}
       | CD WORD           {cd_cmd($2);}
       | PWD               {pwd_cmd();} 
       | ECHO_CMD WORD     {echo_cmd($2);}
       | BYE               {bye_cmd();} 
       | SETENV WORD WORD  {setenv_cmd($2, $3);}
       | UNSETENV WORD     {unsetenv_cmd($2);}
       | PRINTENV          {printenv_cmd();}
       | PRINTENV STDOUT1 WORD  {pipeenv_cmd($3, 0);}
       | PRINTENV STDOUT2 WORD  {pipeenv_cmd($3, 1);}
       | ALIAS             {printalias_cmd();}
       | ALIAS STDOUT1 WORD  {pipealias_cmd($3, 0);}
       | ALIAS STDOUT2 WORD  {pipealias_cmd($3, 1);}
       | ALIAS WORD WORD   {setalias_cmd($2, $3);}
       | UNALIAS WORD      {unalias_cmd($2);}
       | INVALID           {invalid_arguments();}
       | INVALIDALIAS      {invalid_alias();}
       | cmd arglist inputfile outputfile1 errfile background {handle_cmd($1, $2, $3, $4, 0, $5, 0, $6);}
       | cmd arglist inputfile outputfile2 errfile background {handle_cmd($1, $2, $3, $4, 1, $5, 0, $6);}
       | cmd arglist inputfile outputfile1 STDERRO background {handle_cmd($1, $2, $3, $4, 0, NULL, 1, $6);}
       | cmd arglist inputfile outputfile2 STDERRO background {handle_cmd($1, $2, $3, $4, 1, NULL, 1, $6);}
       |                   {;}
       ;

cmd: WORD                  {$$ = subAlias($1);}

arglist: WORD              {addToArglist($1); $$ = getArglistString();}
       | arglist WORD      {addToArglist($2); $$ = getArglistString();}
       |                   {$$ = NULL;}
       ;

inputfile: STDIN WORD      {$$ = $2;}
       |                   {$$ = NULL;}
       ;

outputfile1: STDOUT1 WORD   {$$ = $2;}
       |                    {$$ = NULL;} 
       ;

outputfile2: STDOUT2 WORD   {$$ = $2;}
       |                    {$$ = NULL;} 
       ;

errfile: STDERRF            {;}
       |                    {$$ = NULL;} 
       ;

background: '&'            {$$ = 1;}
       |                   {$$ = 0;} 
       ;

%%

int yyerror(char *s) {
    unknown_command();
    return 0;
}