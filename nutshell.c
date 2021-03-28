#include <stdio.h>
#include <unistd.h>
#include "parse-spec.tab.h"

int main(void) {
    while(1) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("nutshell:%s>> ", cwd);
        yyparse();
    }
    return 0;
}