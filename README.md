# COP4600 Nutshell Term Project

## Sean Minchin and Carlos Carrasquillo

## Description

This nutshell is a command interpreter for a Korn shell-like command language in written in C/C++. The nutshell relies on Flex and Bison for both lexical analysis and grammer parsing. All commands are designed to run natively in the Unix kernel.

### Non-implemented features

* n/a

### Implemented features

* Built-in commands, including alias infinite loop detection
* Non-build in commands, with and without args
* Output redirection with built-in commands
* I/O Redirection with non-built-in commands
* Piping commands
* Piping commands with I/O redirection
* Environment variable expansion
* Alias expansion
* Wildcard matching
* Tilde expansion
* File name completions

### Task Distribution

Sean: I implemented I/O redirection (including input, output, and error redirection), piping an arbitrary number of commands together (with I/O redirection), wildcard matching, running commands using fork() and exec(), the Flex and Bison for detecting and processing I/O redirection, running non-built in commands with arguments, output redirection for setenv and alias, running processes in the background, and contributed to alias handling (doing the check for alias infinite loop expansion and handling nested aliases). I also ported over the original code we had in C over to C++ in order to use its data structures and library functions, and I also updated the Makefile to work in C++.

Carlos Carrasquillo: I developed the program's basic architecture, including the program structure (and Makefile) and implementations of the lexer and parser using both Flex and Bison. The implementation of the lexer includes tasks such as handling built-in commands, "strings", ${environmental variables}, aliases, words, and other tokens (some of these were also bug-tested and revised by Sean). I also developed some of the build-in functions and their functionality, including alias, setenv, cd. I also completed the extra credit for tilde expansion and file name completion.

### GitHub

https://github.com/CarlosCarras/nutshell
