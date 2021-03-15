%{
#include <stdio.h>
#include "parse-spec.tab.h"

void yyerror(char *);
%}

word            [^ \\\<\>\|\"\&\t\n]+|\"[^\"]*\"
metacharacter   [\\\<\>\|\"\&]
whitespace      [ \t\n]+

%%

{word}          {printf("Word: %s\n", yytext); yylval = *yytext; return WORD;}
{metacharacter} {printf("Metacharacter: %s\n", yytext); return *yytext;}
{whitespace}    ;
.               yyerror("Unknown character");

%%

int yywrap(void) {
    return 1;
}