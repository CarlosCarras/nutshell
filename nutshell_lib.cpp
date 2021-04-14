extern "C" {
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
}
#include "nutshell_lib.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <sstream>

using namespace std;

evTable_t varTable;
aTable_t aliasTable;
char cwd[PATH_MAX];
char args[MAX_ARGLIST_LEN];

int run_cmd(char* const args[]) {
    string dir = "/bin/";
    dir.append(args[0]);
    
    pid_t pid = fork();
    if(pid == -1) {
        cout << "ERROR: cannot create child" << endl;
        return 1;
    }

    if(pid == 0) {
        execv(dir.c_str(), args);
        cout << "ERROR: cannot run command" << endl;
        return -1;
    } else {
        int status;
        if(waitpid(pid, &status, 0) == -1) {
            cout << "ERROR: awaiting child process" << endl;
            return 1;
        }
    }

    return 0;
}

// int run_cmd(char* cmd) {
//     cout << "command: " << cmd << endl;
//     int fd[2];
//     if(pipe(fd) == -1) {
//         cout << "error creating pipe" << endl;
//         return 1;
//     }
//     auto pid = fork();
//     if(pid == -1) {
//         cout << "error creating child process" << endl;
//         return 1;
//     }

//     if(pid == 0) {
//         // child process
//         cout << "child" << endl;
//         dup2(fd[1], STDOUT_FILENO);
//         close(fd[0]);
//         close(fd[1]);

//         string temp(cmd);
//         auto numArgs = count(temp.begin(), temp.end(), ' ') + 1;
//         char* argv[numArgs+1];

//         size_t found = 0, index = 0;
//         while(found != string::npos) {
//             argv[index++] = &cmd[found];
//             found = temp.find(" ");
//         }
//         argv[numArgs] = (char*)NULL;

//         cout << "argv[]: ->";
//         for(size_t i = 0; i < index; ++i) {
//             cout << argv[i] << "->";
//         }
//         cout << endl;

//         string path = "/bin/" + string(argv[0]);

//         // vector<string> args;
//         // string cmd_string(cmd);
//         // stringstream ss(cmd_string);
//         // string token;
//         // while(getline(ss, token, ' ')) {
//         //     args.push_back(token);
//         // }

//         // string path = "/bin/";
//         // path.append(args.at(0));

//         // char* argv[args.size()+1];
//         // for(size_t i = 0; i < args.size(); ++i) {
//         //     argv[i] = (char*)(args[i].c_str());
//         // }
//         // argv[args.size()] = (char*)NULL;

//         int childRetCode = execv(path.c_str(), argv);

//         if(childRetCode == -1) {
//             // error
//             cout << "error executing command" << endl;
//             return -1;
//         }
//     } else {
//         close(fd[1]);
//         char output[1024];
//         auto size = read(fd[0], output, sizeof(output)*sizeof(*output));

//         output[size*sizeof(*output)] = '\0';

//         cout << "out: " << output << endl;
//         while(wait(nullptr) != pid);
//     }
//     return 0;
// }

/**************************** Arglist *****************************/

vector<string> argsList;
char argsString[512];

void restart() {
    argsList.clear();
}

void addToArglist(const char* word) {
    argsList.emplace_back(word);
}

char* getArglistString() {
    cout << "check 0" << endl;
    size_t index = 0;
    for(const auto& arg : argsList) {
        for(const auto& c : arg) {
            argsString[index++] = c;
        }
        argsString[index++] = ' ';
    }
    // replace last space delimiter with null char
    argsString[index-1] = '\0';

    return argsString;
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
