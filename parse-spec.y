%{
    #include <cstdlib>
	#include <cstdio>
    #include <cstring>
    #include <string>
    #include "commands.h"
    #include "nutshell_lib.h"
    
	int yylex(void);
    void yyerror(char*);

    extern int yylineno;
%}

%start input

%union 
{
    char* string;
    int boolean;
}

%token CD BYE PWD SETENV UNSETENV PRINTENV ALIAS UNALIAS PRINTALIAS INVALIDALIAS ECHO_CMD
%token END INVALID

%token<string> WORD

%type<string> cmd arglist
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
       | INVALID           {printf("error: invalid arguements.\n");}
       | INVALIDALIAS      {printf("error: invalid alias name.\n");}
       | cmd arglist       {handle_cmd($1, $2, NULL, NULL, NULL, NULL, 0);} // NOT WORKING!
       |                   {unknown_command();}
       ;

cmd: WORD                  {$$ = subAlias($1);}

arglist: WORD              {addToArglist($1); $$ = args;}
       | arglist WORD      {addToArglist($2); $$ = args;}
       |                   {$$ = NULL;}
       ;

background: '&'            {$$ = 1;}
       |                   {$$ = 0;} 
       ;

%%

void yyerror(char *s) {
    // fprintf(stderr, "yyerror: line %d: %s\n", yylineno, s);
    unknown_command();
}