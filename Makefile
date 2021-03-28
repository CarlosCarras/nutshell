# Simple Makefile

CC=/usr/bin/cc
CFLAGS=#-Wall

all:  flex-config bison-config parse-spec scan-spec nutshell nutshell_cmds nutshell-out

flex-config:
	flex scan-spec.l

bison-config:
	bison -d parse-spec.y

scan-spec:  lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c -o scan-spec.lex.o


parse-spec:  parse-spec.tab.c 
	$(CC) $(CFLAGS) -c parse-spec.tab.c -o parse-spec.y.o


nutshell_cmds:  nutshell_cmds.h nutshell_cmds.c
	$(CC) $(CFLAGS) -g -c nutshell_cmds.c -o nutshell_cmds.o

nutshell:  nutshell.c
	$(CC) $(CFLAGS) -g -c nutshell.c -o nutshell.o 

nutshell-out: 
	$(CC) $(CFLAGS) -o nutshell nutshell_cmds.o nutshell.o scan-spec.lex.o parse-spec.y.o -ll -lm -lfl

clean: 
	rm -rf *.o *.yy.c *.tab.*
	rm -rf nutshell