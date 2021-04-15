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
%token INPIPE OUTPIPE
%token END INVALID WHITESPACE

%token<string> WORD

%type<string> cmd arglist inputfile outputfile
%type<boolean> background

%%

input: command END         {return 1;}

command: CD                {cd_home();}
       | CD WORD           {cd_cmd($2);} 
       | PWD               {pwd_cmd();} 
       | ECHO_CMD WORD     {echo_cmd($2);}
       | BYE               {bye_cmd();} 
       | SETENV WORD WORD  {setenv_cmd($2, $3);}
       | UNSETENV WORD     {unsetenv_cmd($2);}
       | PRINTENV          {printenv_cmd();}
       | ALIAS             {printalias_cmd();}
       | ALIAS WORD WORD   {setalias_cmd($2, $3);}
       | UNALIAS WORD      {unalias_cmd($2);}
       | INVALID           {invalid_arguments();}
       | INVALIDALIAS      {invalid_alias();}
       | cmd arglist inputfile outputfile background {handle_cmd($1, NULL, $2, $3, $4, NULL, $5);} // NOT WORKING!
       |                   {;}
       ;

cmd: WORD                  {$$ = subAlias($1);}

arglist: WORD              {addToArglist($1); $$ = getArglistString();}
       | arglist WORD      {addToArglist($2); $$ = getArglistString();}
       |                   {$$ = NULL;}
       ;

inputfile: INPIPE WORD     {$$ = $2;}
       |                   {$$ = NULL;}
       ;

outputfile:OUTPIPE WORD    {$$ = $2;}
       |                   {$$ = NULL;} 
       ;

background: '&'            {$$ = 1;}
       |                   {$$ = 0;} 
       ;

%%

int yyerror(char *s) {
    unknown_command();
    return 0;
}