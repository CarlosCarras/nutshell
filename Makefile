# Simple Makefile

CC=/usr/bin/cc
CCC=g++

CFLAGS=-Wall 
CPPFLAGS=-std=c++17 $(CFLAGS)

all:  flex-config bison-config nutshell_lib commands scan-spec parse-spec nutshell nutshell-out #flex-config bison-config parse-spec scan-spec nutshell commands nutshell_lib nutshell-out

flex-config:
	flex scan-spec.l

bison-config:
	bison -d parse-spec.y

scan-spec:  lex.yy.c
	$(CCC) $(CPPFLAGS) -c lex.yy.c -o scan-spec.lex.o

parse-spec:  parse-spec.tab.c 
	$(CCC) $(CPPFLAGS) -c parse-spec.tab.c -o parse-spec.y.o

commands:  commands.h commands.cpp
	$(CCC) $(CPPFLAGS) -c commands.cpp -o commands.o

nutshell_lib:  nutshell_lib.h nutshell_lib.cpp
	$(CCC) $(CPPFLAGS) -c nutshell_lib.cpp -o nutshell_lib.o

nutshell:  nutshell.cpp
	$(CCC) $(CPPFLAGS) -c nutshell.cpp -o nutshell.o

nutshell-out: 
	$(CCC) $(CPPFLAGS) -o nutshell nutshell_lib.o commands.o nutshell.o scan-spec.lex.o parse-spec.y.o -ll -lm

clean: 
	rm -rf *.o *.yy.c *.tab.*
	rm -rf nutshell