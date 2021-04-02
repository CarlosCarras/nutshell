%{
#include <stdio.h>
#include <string.h>
#include "nutshell_lib.h"
#include "parse-spec.tab.h"



char buf[100];
char *s;

%}

%array
%option noyywrap
%x STRING

alias           \$\{(.*?)\}
quote           ["]
word            [^ \\<>|="&\t\n]+
metachar        [\\<>|"&]
newline         "\n"
whitespace      [ \t\n]+

%%

^cd             {return CD;}
^bye            {return BYE;}
^pwd            {return PWD;}
^setenv         {return SETENV;}
^alias		    {return ALIAS;}
^echo           {return ECHO_CMD;}

{alias}         {
                    memmove(yytext, yytext+2, strlen(yytext));  // eliminate first two chars, '${'
                    yytext[strlen(yytext)-1] = '\0';            // eliminate last char, '}'
                    
                    if(isAlias(yytext)) {
                        char *yycopy = strdup(subAliases(yytext));
                        for (int i = strlen(yycopy)-1; i >= 0; --i) {
                            unput(yycopy[i]);
                        }
                        free(yycopy);
                        return WORD;
                    }
                }

{quote}		    {BEGIN STRING; s = buf;};
<STRING>\\n     {*s++ = '\n';}
<STRING>\\t     {*s++ = '\t';}
<STRING>\\\"    {*s++ = '\"';}
<STRING>{quote} {*s = 0; BEGIN INITIAL;}
<STRING>\n      {return INVALID;}
<STRING>.       {*s++ = *yytext; return WORD;}

{word}          {yylval.string=strdup(yytext); return WORD;}
{newline}       {return NEWLINE;}
{metachar}      {return (int)yytext[0];}
{whitespace}    {;}
.               {return INVALID;}

%%