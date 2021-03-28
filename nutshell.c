#include <stdio.h>
#include "parse-spec.tab.h"

int main(void) {
    while(1) {
        printf(">> ");
        yyparse();
    }
    return 0;
}