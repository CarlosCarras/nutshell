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

envvar          \$\{(.*?)\}
quote           ["]
word            [^ \\<>|="&\t\n]+
metachar        [\\<>|"&]
newline         "\n"
whitespace      [ \t\n]+
inpipe          "<"
outpipe         ">"

%%

^cd             {return CD;}
^pwd            {return PWD;}
^echo           {return ECHO_CMD;}
^bye            {return BYE;}
^setenv         {return SETENV;}
^unsetenv       {return UNSETENV;}
^printenv		{return PRINTENV;}
^alias		    {return ALIAS;}
^unalias	    {return UNALIAS;}

{envvar}        {
                    memmove(yytext, yytext+2, strlen(yytext));  // eliminate first two chars, '${'
                    yytext[strlen(yytext)-1] = '\0';            // eliminate last char, '}'
                    
                    if(isVar(yytext)) {
                        char *yycopy = strdup(subVar(yytext));
                        for (int i = strlen(yycopy)-1; i >= 0; --i) {
                            unput(yycopy[i]);
                        }
                        free(yycopy);
                        return WORD;
                    } else {
                        yylval.string=strdup(yytext);
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

{inpipe}        {return INPIPE;}
{outpipe}       {return OUTPIPE;}
{newline}       {return END;}

{word}          {
                    printd("Word:", yytext);
                    char *yycopy = strdup(yytext);
                    if (strlen(yycopy) > MAX_WORD_LEN) {
                        printf("error: words cannot exceed %d characters.", MAX_WORD_LEN);
                        return INVALID;
                    }

                    if (isPattern(yycopy)) {
                        yylval.string=strdup(subPattern(yytext)); 
                    } else {
                        yylval.string=strdup(yytext); 
                    }
                    free(yycopy);
                    return WORD;
                }

{metachar}      {return (int)yytext[0];}
{whitespace}    {;}
.               {return INVALID;}

%%