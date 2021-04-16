#include "parse-spec.tab.h"
#include "nutshell_lib.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

using namespace std;

int main() {
    setStartupVars();      // sets HOME, PATH variables
    restart();
    run_cmd((char* const[]){ (char*)"clear", (char*)NULL }); // clear terminal window

    while(1) {
        getcwd(cwd, sizeof(cwd));
        cout << BLUE "nutshell" RESET ":" GREEN << cwd << WHITE ">> " RESET;
        
        yyparse();
        usleep(2000);       // delay to fix race conditions
    }
    return 0;
}