#include "parse-spec.tab.h"
#include "nutshell_lib.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

int main() {
    setStartupVars();      // sets HOME, PATH variables
    system("clear");       // clears terminal window

    while(1) {
        getcwd(cwd, sizeof(cwd));
        printf(BLUE "nutshell" RESET ":" GREEN "%s" WHITE ">> " RESET, cwd);
        
        yyparse();
        usleep(1000);       // delay to fix race conditions
    }
    return 0;
}