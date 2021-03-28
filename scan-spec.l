%{
#include <stdio.h>
#include <string.h>
#include "parse-spec.tab.h"

void yyerror(char *);
%}

word            [^ \\<>|\="&\~\.\t\n]+|(\"[^"\\]*(?:\\.[^"\\]*)*\")
metacharacter   [\\<>|"&]
whitespace      [ \t\n]+
dot             [\.]
tilde           [\~]

%%

{word}          {yylval.string=strdup(yytext); return WORD;}
{dot}           {return DOT;}
{tilde}         {return TILDE;}
{metacharacter} {return (int)yytext[0];}
{whitespace}    {;}
.               {return UNDEFINED;}

%%

int yywrap(void) {
    return 1;
}