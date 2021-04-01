# Simple Makefile

CC=/usr/bin/cc
CFLAGS=#-Wall

all:  flex-config bison-config parse-spec scan-spec nutshell commands nutshell_lib nutshell-out

flex-config:
	flex scan-spec.l

bison-config:
	bison -d parse-spec.y

scan-spec:  lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c -o scan-spec.lex.o


parse-spec:  parse-spec.tab.c 
	$(CC) $(CFLAGS) -c parse-spec.tab.c -o parse-spec.y.o


commands:  commands.h commands.c
	$(CC) $(CFLAGS) -g -c commands.c -o commands.o

nutshell_lib:  nutshell_lib.h nutshell_lib.c
	$(CC) $(CFLAGS) -g -c nutshell_lib.c -o nutshell_lib.o

nutshell:  nutshell.c
	$(CC) $(CFLAGS) -g -c nutshell.c -o nutshell.o 

nutshell-out: 
	$(CC) $(CFLAGS) -o nutshell nutshell_lib.o commands.o nutshell.o scan-spec.lex.o parse-spec.y.o -ll -lm -lfl

clean: 
	rm -rf *.o *.yy.c *.tab.*
	rm -rf nutshell