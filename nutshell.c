#include <stdio.h>
#include "parse-spec.tab.h"

int main(void) {
    while(1) {
        yyparse();
    }
    return 0;
}