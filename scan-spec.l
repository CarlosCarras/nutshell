%{
#include <stdio.h>
#include <string.h>
#include "parse-spec.tab.h"

void yyerror(char *);

char buf[100];
char *s;
%}

%x STRING

quote           ["]
notquote        [^"]

word            [^ \\<>|="&\t\n]+
metachar        [\\<>|"&]
whitespace      [ \t\n]+
dot             [\.]
tilde           [\~]

%%

^cd             {return CD;}
^bye            {return BYE;}
^pwd            {return PWD;}
^setenv         {return SETENV;}

{quote}		    {BEGIN STRING; s = buf;};
<STRING>\\n     {*s++ = '\n';}
<STRING>\\t     {*s++ = '\t';}
<STRING>\\\"    {*s++ = '\"';}
<STRING>\"      {*s = 0; BEGIN INITIAL;}
<STRING>\n      {printf("invalid string"); exit(1);}
<STRING>.       {*s++ = *yytext; return WORD;}

{word}          {yylval.string=strdup(yytext); return WORD;}
{metachar}      {return (int)yytext[0];}
{whitespace}    {;}
.               {return UNDEFINED;}

%%

int yywrap(void) {
    return 1;
}