extern "C" {
#include <errno.h>
#include <unistd.h>
#include <string.h>
}
#include "nutshell_lib.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <streambuf>

using namespace std;

evTable_t varTable;
aTable_t aliasTable;
char cwd[PATH_MAX];

/*************************** Var Table ***************************/

void setVar(char* name, char* word) {
    varTable.var.emplace_back(name);
    varTable.word.emplace_back(word);
}

void setStartupVars() {
    getcwd(cwd, sizeof(cwd));
    
    setVar((char*)"HOME", cwd);
    setVar((char*)"PATH", (char*)".:/bin");
}

char* subVar(char* var) {
    auto it = find(varTable.var.begin(), varTable.var.end(), var);
    if(it == varTable.var.end()) {
        return var;
    }

    auto index = distance(varTable.var.begin(), it);
    return (char*)(varTable.word.at(index).c_str());
}

int isVar(char* var) {
    return any_of(varTable.var.begin(), varTable.var.end(), [var](const string& s){ return s.compare(var) == 0; });
}

/************************** Alias Table **************************/

void setAlias(char* name, char* word) {
    aliasTable.name.emplace_back(name);
    aliasTable.word.emplace_back(word);
}

char* subAlias(char* name) {
    auto it = find(aliasTable.name.begin(), aliasTable.name.end(), name);
    if(it == aliasTable.name.end()) {
        return name;
    }

    auto index = distance(aliasTable.name.begin(), it);
    return (char*)(aliasTable.word.at(index).c_str());
}

int isAlias(char* name) {
    return any_of(aliasTable.name.begin(), aliasTable.name.end(), [name](const string& s){ return s.compare(name) == 0; });
}

/************************ Pattern Matching ***********************/

int isPattern(char* word) {
    string str(word);
    size_t end = string::npos;
    return str.find("?") != end || str.find("*") != end;
}

char* subPattern(char* word) {
    string filename = "test.txt";
    string cmd1 = "ls " + string(word) + " > " + filename;
    string cmd2 = "rm -f " + filename;

    system(cmd1.c_str());

    /* put all contents of "filename" into std::string outstr. they should
       already be sorted. */
    ifstream t(filename);
    string outstr((istreambuf_iterator<char>(t)),
                   istreambuf_iterator<char>());
    replace(outstr.begin(), outstr.end(), '\n', ' ');

    system(cmd2.c_str());
    printf("%s\n", outstr.c_str());
    return word;
}

/************************ Print Functions ************************/

void printd(const char* desc, const char* val) {
    #ifdef DEBUG_NUTSHELL
        cout << "\tDEBUG: " << desc << ' ' << val << endl;
    #endif // DEBUG_NUTSHELL
}

void printd(const string& desc, const string& val) {
    #ifdef DEBUG_NUTSHELL
        cout << "\tDEBUG: " << desc << ' ' << val << endl;
    #endif // DEBUG_NUTSHELL
}

void printerr() {
    string err = "error: ";

    switch(errno) {
        /* chdir, putenv */
        case EACCES      : { err.append("search permission is denied for one of the components of path."); break; }
        case EFAULT      : { err.append("path points outside your accessible address space."); break; }
        case EIO         : { err.append("an I/O error occurred."); break; }
        case ELOOP       : { err.append("too many symbolic links were encountered in resolving path."); break; }
        case ENAMETOOLONG: { err.append("path is too long."); break; }
        case ENOENT      : { err.append("the directory specified in path does not exist."); break; }
        case ENOMEM      : { err.append("insufficient kernel memory was available."); break; }
        case ENOTDIR     : { err.append("q component of path is not a directory."); break; }
    }

    cout << err << endl;
}
