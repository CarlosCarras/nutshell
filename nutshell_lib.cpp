extern "C" {
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fnmatch.h>
#include <fcntl.h>
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

string getExecPath(const string& path, const string& program) {
    DIR* d;
    struct dirent* dir;
    vector<string> fileList;
    istringstream iss(path);
    string directory;
    while(getline(iss, directory, ':')) {
        d = opendir(directory.c_str());
        if(d) {
            while((dir = readdir(d)) != NULL) {
                fileList.emplace_back(dir->d_name);
            }
            closedir(d);
        }

        for(const auto& f : fileList) {
            if(f == program) {
                string path(directory);
                path.append("/");
                path.append(f);
                return directory + "/" + program;
            }
        }
    }

    return string(program);
}

int executeCommand(char* args[], const char* fileStdIn, int stdIn, const char* fileStdOut, int stdOut, const char* fileStdErr, int stdErr, bool background) {
    pid_t pid = fork();
    if(pid == -1) {
        return 1;
    }

    if(pid == 0) {
        if(stdIn > 0) {
            char* const* tempArg = args;
            size_t argsSize = 0;
            do {
                ++argsSize;
            } while(*tempArg++);

            args[argsSize-1] = (char*)fileStdIn;
            args[argsSize] = (char*)NULL;
        }

        if(stdOut > 0) {
            // handle standard output
            int flags = O_RDWR;
            switch(stdOut) {
                case 1: flags |= O_CREAT; break;
                case 2: flags |= O_APPEND; break;
            }

            int fdOut = open(fileStdOut, flags, S_IRUSR | S_IWUSR);

            dup2(fdOut, STDOUT_FILENO);

            close(fdOut);
        }

        string exe(args[0]);
        string path = getExecPath(getPath(), exe);
        execv(path.c_str(), args);
        
        string err(args[0]);
        err.append(": ");
        char* const* arg = &args[1];
        while(*arg != NULL) {
            err.append(*arg++);
        }
        err.append(": ");
        err.append(strerror(errno));
        err.append("\n");

        switch(stdErr) {
            // handle std error
            case 0: {
                cout << err;
                break;
            }
            case 1: {
                int fdErr = open(fileStdErr, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                write(fdErr, err.c_str(), err.length());
                close(fdErr);
                break;
            }
            case 2: {
                int fdErr = open(fileStdOut, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
                write(fdErr, err.c_str(), err.length());
                close(fdErr);
                break;
            }
        }

        return -1;
    } else if(!background) {
        int status;
        if(waitpid(pid, &status, 0) == -1) {
            return 1;
        }
    }

    return 0;
}

int run_cmd(char* const args[]) {
    string exe(args[0]);
    string path = getExecPath(getPath(), exe);
    
    pid_t pid = fork();
    if(pid == -1) {
        return 1;
    }

    if(pid == 0) {
        execv(path.c_str(), args);
        return -1;
    } else {
        int status;
        if(waitpid(pid, &status, 0) == -1) {
            return 1;
        }
    }

    return 0;
}

int write_to_file(const char* file, const char* data, size_t len, int append) {
    pid_t pid = fork();
    if(pid == -1) {
        return 1;
    }

    if(pid == 0) {
        int flags = O_RDWR;
        mode_t mode = S_IRUSR | S_IWUSR;
        if(append) {
            flags |= O_APPEND;
        } else {
            flags |= O_CREAT;
        }

        int fd = open(file, flags, mode);
        write(fd, data, len);
        close(fd);
    } else {
        int status;
        if(waitpid(pid, &status, 0) == -1) {
            return 1;
        }
    }

    return 0;
}

// int redir_stdout(const char* file, char* const args[], int append) {
//     pid_t pid = fork();
//     if(pid == -1) {
//         return 1;
//     }

//     if(pid == 0) {
//         int flags = O_RDWR | (append ? O_APPEND : O_CREAT);
//         mode_t mode = S_IRUSR | S_IWUSR;

//         int fd = open(file, flags, mode);

//         dup2(fd, STDOUT_FILENO);

//         close(fd);

//         string path = "/bin/";
//         path.append(args[0]);

//         execv(path.c_str(), args);
//         return -1;
//     } else {
//         int status;
//         if(waitpid(pid, &status, 0) == -1) {
//             return 1;
//         }
//     }

//     return 0;
// }

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

const string& getPath() {
    auto it = find(varTable.var.begin(), varTable.var.end(), "PATH");
    auto index = distance(varTable.var.begin(), it);
    return varTable.word.at(index);
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

char patternBuffer[1024];

int isPattern(char* word) {
    string str(word);
    auto end = string::npos;
    return str.find("?") != end || str.find("*") != end;
}

char* subPattern(const char* pattern) {
    DIR* d;
    struct dirent* dir;
    vector<string> fileList;

    d = opendir((const char*)".");
    if(d) {
        while((dir = readdir(d)) != NULL) {
            fileList.emplace_back(dir->d_name);
        }
        closedir(d);
    }

    string matchedWildcards;
    for(const auto& f : fileList) {
        if(fnmatch(pattern, f.c_str(), 0) == 0) {
            // match with wildcard against file name
            matchedWildcards.append(f);
            matchedWildcards.append(" ");
        }
    }

    if(matchedWildcards.empty()) {
        // no wilcard matches so just return original string
        strcpy(patternBuffer, pattern);
        return patternBuffer;
    }
    matchedWildcards.pop_back(); // remove last space

    // cout << '[' << matchedWildcards << ']' << endl;

    if(matchedWildcards.length()+1 > 1024) {
        cout << "Error: wildcards size larger than buffer" << endl;
        strcpy(patternBuffer, pattern);
        return patternBuffer;
    }
    
    strcpy(patternBuffer, matchedWildcards.c_str());
    return patternBuffer;
}

/************************ Print Functions ************************/

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
