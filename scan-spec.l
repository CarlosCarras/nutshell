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
word            [^ \x1B\\<>="&\t\n]+
escapedword     [^ \x1B\\<>|="&\t\n]+[\x1B]
metachar        [\\<>|"&]
newline         "\n"
whitespace      [ \t]
stdin           <+
stdout1         >+
stdout2         >>+
stderrf         2>[^ "&\t\n]+
stderro         2>&1+

%%

<<EOF>>         {return FILEEND;}

^cd             {return CD;}
^pwd            {return PWD;}
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
                    } else {
                        yylval.string=strdup(yytext);
                        return WORD;
                    }
                }

{quote}		     {BEGIN STRING; s = buf;};
<STRING>\\n      {*s++ = '\n';}
<STRING>\\t      {*s++ = '\t';}
<STRING>{envvar} {
                    memmove(yytext, yytext+2, strlen(yytext));  // eliminate first two chars, '${'
                    yytext[strlen(yytext)-1] = '\0';            // eliminate last char, '}'
                    
                    if(isVar(yytext)) {
                        char *yycopy = strdup(subVar(yytext));
                        for (int i = strlen(yycopy)-1; i >= 0; --i) {
                            unput(yycopy[i]);
                        }
                        free(yycopy);
                    } else {
                        yylval.string=strdup(yytext);
                        return WORD;
                    }
                 }
<STRING>\\\"     {*s++ = '\"';}
<STRING>{quote}  {*s = 0; BEGIN INITIAL; yylval.string=strdup(buf); return WORD;}
<STRING>\n       {return INVALID;}
<STRING>.        {*s++ = *yytext;}

{stdin}         {return STDIN;}
{stdout2}       {return STDOUT2;}
{stdout1}       {return STDOUT1;}
{stderro}       {return STDERRO;}
{stderrf}       {
                    char *yycopy = strdup(yytext);
                    memmove(yycopy, yycopy+2, strlen(yycopy));
                    yylval.string=strdup(yycopy);
                    free(yycopy);
                    return STDERRF;
                }

{newline}       {return END;}

{escapedword}   {
                    yytext[strlen(yytext)-1] = '\0';            // eliminate last char, '}'
                    char* result = strdup(handle_esc(yytext));

                    for (int i = strlen(result)-1; i >= 0; --i) {
                            unput(result[i]);
                    }

                    free(result);
                }

{word}          {
                    char *yycopy = strdup(yytext);
                    char* postTildeSub;

                    if (strlen(yycopy) > MAX_WORD_LEN) {
                        printf("error: words cannot exceed %d characters.", MAX_WORD_LEN);
                        return INVALID;
                    }

                    if (requiresTildeExp(yycopy)) {
                        postTildeSub=strdup(subTilde(yycopy));
                        if (postTildeSub[0] == '\0') {};
                    } else {
                        postTildeSub=strdup(yycopy);
                    }

                    if (isPattern(postTildeSub)) {
                        yylval.string=strdup(subPattern(postTildeSub));
                    } else {
                        yylval.string=strdup(postTildeSub); 
                    }

                    free(yycopy);
                    free(postTildeSub);  
                    return WORD;
                }

{whitespace}    {;}
{metachar}      {return (int)yytext[0];}
.               {return INVALID;}

%%