extern "C" {
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <pwd.h>
}
#include "nutshell_lib.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iterator>

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

    return "";
}

int executeCommand(command_t command) {
    auto [argsList, fileStdIn, fileStdOut, fileStdErr, inFlag, outFlag, errFlag, background] = command;

    int fdNext[2], fdPrev[2];

    for(const auto& args : argsList) {
        const bool hasNextCmd = args != argsList.back();
        const bool hasPrevCmd = args != argsList.front();

        if(hasNextCmd) {
            pipe(fdNext);
        }

        pid_t pid = fork();
        if(pid == -1) {
            return 1;
        }

        if(pid == 0) {
            if(hasPrevCmd) {
                dup2(fdPrev[STDIN_FILENO], STDIN_FILENO);
                close(fdPrev[STDIN_FILENO]);
                close(fdPrev[STDOUT_FILENO]);
            } else {
                // if we are the first command
                if(inFlag > 0) {
                    int fdIn = open(fileStdIn, O_RDONLY);
                    dup2(fdIn, STDIN_FILENO);
                    close(fdIn);
                }
            }
            if(hasNextCmd) {
                close(fdNext[STDIN_FILENO]);
                dup2(fdNext[STDOUT_FILENO], STDOUT_FILENO);
                close(fdNext[STDOUT_FILENO]);
            } else {
                // if we are at the last command
                if(outFlag > 0) {
                    // handle standard output
                    int flags = O_WRONLY;
                    switch(command.outFlag) {
                        case 1: flags |= O_CREAT; break;
                        case 2: flags |= O_APPEND; break;
                    }

                    int fdOut = open(fileStdOut, flags, S_IRUSR | S_IWUSR);
                    dup2(fdOut, STDOUT_FILENO);
                    if(errFlag == 2) {
                        dup2(fdOut, STDERR_FILENO);
                    }
                    close(fdOut);
                }

                if(errFlag == 1) {
                    int fdErr = open(fileStdErr, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(fdErr, STDERR_FILENO);
                    close(fdErr);
                }
            }

            string file(args.at(0));
            if((file.at(0) == '.' && file.at(1) == '/') || file.at(0) == '/') {
                execv(file.c_str(), args.data());
            } else {
                string path = getExecPath(getPath(), file);
                execv(path.c_str(), args.data());
            }

            cout << "command [" << string(args.at(0)) << "]: " << strerror(errno) << endl;
            return -1;
        } else {
            if(hasPrevCmd) {
                close(fdPrev[STDIN_FILENO]);
                close(fdPrev[STDOUT_FILENO]);
            }
            if(hasNextCmd) {
                fdPrev[STDIN_FILENO] = fdNext[STDIN_FILENO];
                fdPrev[STDOUT_FILENO] = fdNext[STDOUT_FILENO];
            }

            if(!background) {
                int status;
                if(waitpid(pid, &status, 0) == -1) {
                    return 1;
                }
            }
        }
    }

    if(argsList.size() > 1) {
        close(fdPrev[STDIN_FILENO]);
        close(fdPrev[STDOUT_FILENO]);
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
    static string alias;

    alias = string(name);
    while(true) {
        auto it = find(aliasTable.name.begin(), aliasTable.name.end(), alias);
        if(it == aliasTable.name.end()) {
            return (char*)(alias.c_str());
        }

        auto index = distance(aliasTable.name.begin(), it);
        alias = aliasTable.word.at(index);
    }
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

char* subPattern(const char* word) {
    DIR* d;
    struct dirent* dir;
    vector<string> fileList;

    /* getting the path and the pattern from the input */
    string pattern, path;
    string input(word);
    string copy = input;
    
    reverse(copy.begin(), copy.end());
    size_t startOfTarget = copy.find_first_of("/");

    if (startOfTarget == string::npos) {
        startOfTarget = input.size();
        path = ".";
    } else {
        path = input.substr(0, input.size()-startOfTarget);
    }
    pattern = input.substr(input.size() - startOfTarget);

    d = opendir((const char*)path.c_str());
    if(d) {
        while((dir = readdir(d)) != NULL) {
            fileList.emplace_back(dir->d_name);
        }
        closedir(d);
    }

    string matchedWildcards;
    for(const auto& f : fileList) {
        if(fnmatch(pattern.c_str(), f.c_str(), 0) == 0) {
            // match with wildcard against file name
            matchedWildcards.append(f);
            matchedWildcards.append(" ");
        }
    }

    if(matchedWildcards.empty()) {
        // no wilcard matches so just return original string
        strcpy(patternBuffer, pattern.c_str());
        return patternBuffer;
    }
    matchedWildcards.pop_back(); // remove last space

    // cout << '[' << matchedWildcards << ']' << endl;

    if(matchedWildcards.length()+1 > 1024) {
        cout << "error: wildcards size larger than buffer" << endl;
        strcpy(patternBuffer, pattern.c_str());
        return patternBuffer;
    }
    
    strcpy(patternBuffer, matchedWildcards.c_str());
    return patternBuffer;
}

char* subPattern_NoDirChange(const char* word) {
    /* getting the path and the pattern from the input */
    string pattern, path;
    string input(word);
    string copy = input;
    
    reverse(copy.begin(), copy.end());
    size_t startOfTarget = copy.find_first_of("/");

    if (startOfTarget == string::npos) {
        startOfTarget = input.size();
        path = "./";
    } else {
        path = input.substr(0, input.size()-startOfTarget);
    }

    char* subbedPattern = strdup(subPattern(input.c_str()));

    string dir = path + string(subbedPattern);
    strcpy(patternBuffer, dir.c_str());
    return patternBuffer;

}

/************************ Tilde Expansion ************************/
char subbedTildeExpansion[1024];

int requiresTildeExp(char* word) {
    return word[0] == '~';
}

char* subTilde(const char* word) {
    string str(word);
    string user;
    struct passwd *p;

    std::size_t found = str.find('/');
    if (found == string::npos) user = str.substr(1,found);
    else user = str.substr(1,found-1);
    str.erase(0,found);

    if (user.empty()) {
        str = string(subVar((char*)"HOME")) + str;
    } else {
        if ((p = getpwnam(user.c_str())) == NULL) {
            str.clear();
        } else {
            str = p->pw_dir + str; 
        }
    }

    strcpy(subbedTildeExpansion, str.c_str());
    return subbedTildeExpansion;
}

/************************ Tilde Expansion ************************/

/********************** Filename Completion **********************/
char subbedEscapedExpansion[1024];

char* handle_esc(char* word) {
    if (requiresTildeExp(word)) {
        vector<string> candidateList;
        fstream fs;

        string incoming(word);
        size_t endOfUser = incoming.find("/");
        if (endOfUser == string::npos) endOfUser = incoming.length();
        string potentialUser = incoming.substr(1, endOfUser-1);  // eliminating '~' from begining of word

        fs.open("/etc/passwd",ios::in);
        if (fs.is_open()) {
            string line, user, home;
            struct passwd *p;
            
            while(getline(fs, line)) {
                user = line.substr(0, line.find(":"));
                if (user.find(potentialUser) == 0) {     // if the pattern matches the start of one of the users...
                    p = getpwnam(user.c_str()); // get the user's home directory
                    candidateList.emplace_back(p->pw_dir);  // save the home directory 
                }
            }
            fs.close();

            if (candidateList.size() == 1) {    // if the result is not ambiguous...
                strcpy(subbedEscapedExpansion, candidateList[0].c_str());
                strcat(subbedEscapedExpansion, incoming.substr(endOfUser).c_str());
                return subbedEscapedExpansion;
            }
        } else {
            cout << "error: unable to open /etc/passwd" << endl;
        }

    } else {
        string pattern = string(word) + "*";
        char* candidates = subPattern_NoDirChange(pattern.c_str());

        istringstream iss(candidates);
        vector<string> candidateList(istream_iterator<string>{iss},
                                     istream_iterator<string>());

        if (candidateList.size() == 1) {
            strcpy(subbedEscapedExpansion, candidateList[0].c_str());
            return subbedEscapedExpansion;
        }
    }
    return word;
}

/********************** Filename Completion **********************/

/************************ Print Functions ************************/

void printerr() {
    string err = "error: ";

    switch(errno) {
        case EACCES      : { err.append("search permission is denied for one of the components of path."); break; }
        case EFAULT      : { err.append("path points outside your accessible address space."); break; }
        case EIO         : { err.append("an I/O error occurred."); break; }
        case ELOOP       : { err.append("too many symbolic links were encountered in resolving path."); break; }
        case ENAMETOOLONG: { err.append("path is too long."); break; }
        case ENOENT      : { err.append("the directory specified in path does not exist."); break; }
        case ENOMEM      : { err.append("insufficient kernel memory was available."); break; }
        case ENOTDIR     : { err.append("a component of path is not a directory."); break; }
        case ENFILE      : { err.append("the maximum number of files was open already in the system."); break; }
        case EMFILE      : { err.append("the maximum number (OPEN_MAX) of files was open already in the calling process."); break; }
        case EINTR       : { err.append("a signal was caught."); break; }
        case ERANGE      : { err.append("insufficient buffer space supplied."); break; }
    }

    cout << err << endl;
}
