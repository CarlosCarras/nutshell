# Simple Makefile

CC=/usr/bin/cc

all:  flex-config bison-config parse-spec scan-spec nutshell nutshell-out

flex-config:
	flex scan-spec.l

bison-config:
	bison -d parse-spec.y

scan-spec:  lex.yy.c
	$(CC) -c lex.yy.c -o scan-spec.lex.o


parse-spec:  parse-spec.tab.c 
	$(CC) -c parse-spec.tab.c -o parse-spec.y.o


nutshell:  nutshell.c
	$(CC) -g -c nutshell.c -o nutshell.o 

nutshell-out: 
	$(CC) -o nutshell nutshell.o scan-spec.lex.o parse-spec.y.o -ll -lm -lfl

clean: 
	rm -rf *.o *.yy.c *.tab.*
	rm -rf nutshell