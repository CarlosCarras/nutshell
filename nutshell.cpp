#include "parse-spec.tab.h"
#include "nutshell_lib.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

// #define BLUE   "\033[0;34m"
// #define GREEN  "\033[0;32m"
// #define CYAN   "\033[0;36m"
// #define WHITE  "\033[0;37m"
// #define PURPLE "\033[0;35m"
// #define RESET  "\x1B[0m"


int main() {
    setStartupVars();      // sets HOME, PATH variables
    system("clear");            // clears terminal window

    while(1) {
        // getcwd(cwd, sizeof(cwd));
        // printf(BLUE "nutshell");
        // printf(WHITE ":");
        // printf(GREEN "%s", cwd);
        // printf(WHITE ">> " RESET);

        yyparse();
    }
    return 0;
}