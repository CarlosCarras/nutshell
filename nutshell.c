#include <stdio.h>
#include <unistd.h>
#include "parse-spec.tab.h"
#include "nutshell_lib.h"

#define BLUE   "\033[0;34m"
#define GREEN  "\033[0;32m"
#define CYAN   "\033[0;36m"
#define WHITE  "\033[0;37m"
#define PURPLE "\033[0;35m"
#define RESET  "\x1B[0m"


int main(void) {
    aliasIndex = 0;
    varIndex = 0;

    while(1) {
        getcwd(cwd, sizeof(cwd));

        printf(BLUE "nutshell");
        printf(WHITE ":");
        printf(GREEN "%s", cwd);
        printf(WHITE ">> " RESET);

        yyparse();
    }
    return 0;
}