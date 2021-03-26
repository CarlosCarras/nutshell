%{
    #include <stdlib.h>
	#include <stdio.h>
    #include <string.h>
    
	int yylex(void);

    void yyerror(char*);
%}

%union 
{
    char* string;
}


%token <string> WORD
%token UNDEFINED

%%

input: 
    | input WORD		{printf("\tWORD: %s\n", $2);}
    | input '&'         {printf("\tMETACHAR: &\n");}
    | input '\\'        {printf("\tMETACHAR: \\\n");}
    | input '<'         {printf("\tMETACHAR: <\n");}
    | input '>'         {printf("\tMETACHAR: >\n");}
    | input '"'         {printf("\tMETACHAR: \"\n");}
    | UNDEFINED         {printf("\tUndefined Character.\n");}
    
%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}