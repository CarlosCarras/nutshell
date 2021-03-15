%{
    #include <stdlib.h>
	#include <stdio.h>
    
	int yylex(void);

    void yyerror(char *);
%}

%token WORD

%%

program: 
    WORD		{printf("YWORD: %d\n", $1);}

%%

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}