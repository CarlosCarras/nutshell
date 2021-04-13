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
char args[MAX_ARGLIST_LEN];

/**************************** Arglist *****************************/

void restart() {
    char *begin = args;
    char *end = begin + sizeof(MAX_ARGLIST_LEN);
    fill(begin, end, '\0');
}

void addToArglist(const char* word) {
    strcat(args, word);
    strcat(args, " ");
}

/**************************** Arglist *****************************/

/*************************** All Tables ***************************/

bool existsInTable(const vector<string>& table, const char* key) {
    return any_of(table.begin(), table.end(), [key](const string& s){ return s.compare(key) == 0; });
}

bool existsInTable(const vector<string>& table, const string& key) {
    return any_of(table.begin(), table.end(), [key](const string& s){ return s == key; });
}

size_t getTableIndex(const vector<string>& table, const char* key) {
    auto it = find(table.begin(), table.end(), key);
    return distance(table.begin(), it);
}

size_t getTableIndex(const vector<string>& table, const string& key) {
    auto it = find(table.begin(), table.end(), key);
    return distance(table.begin(), it);
}

void removeTableIndex(vector<string>& table, size_t index) {
    table.erase(table.begin() + index);
}

/*************************** All Tables ***************************/

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
    auto end = string::npos;
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
    cout << outstr << endl;
    return word;

    // char* out_str_raw = strdup(outstr.c_str());
    // printf("%s\n", out_str_raw);
    // return word;
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
