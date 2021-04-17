#include "commands.h"
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

void cd_home() {
    cd_cmd(subVar((char*)"HOME"));
}

void cd_cmd(char* dest) {
    int status = chdir(dest);
    if (status < 0) { printerr(); }
}

void pwd_cmd() {
    getcwd(cwd, sizeof(cwd));
    cout << cwd << endl;
}

void echo_cmd(char* val) {
    cout << val << endl;
}

void bye_cmd() {
    exit(EXIT_SUCCESS);
}

/********************* Environment Variables *********************/

void envexp_cmd(char* var) {
    char* val = getenv(var);
    cout << val << endl;
}

void setenv_cmd(char* var, char* word) {
    if(strcmp(var, word) == 0) {
        /* if variable == word */
        cout << "error: setting variable " << var << " would create a loop." << endl;
        return;
    }

    if(existsInTable(varTable.var, var)) {
        auto index = getTableIndex(varTable.var, var);
        varTable.word.at(index) = word;
        return;
    }

    /* else the variable is new, and must be added to the table */
    setVar(var, word);

    printd("New Variable Name> ", var);
    printd("New Variable Val> ", word);
}

void unsetenv_cmd(char* var) {
    if(!existsInTable(varTable.var, var)) {
        cout << "error: environment variable " << var << " does not exist in table" << endl;
        return;
    }

    if(strcmp(var, "PATH") == 0 || strcmp(var, "HOME") == 0) {
        cout << "error: cannot unset environment variable " << var << endl;
        return;
    }

    auto index = getTableIndex(varTable.var, var);
    removeTableIndex(varTable.var, index);
    removeTableIndex(varTable.word, index);
}

string getEnvString() {
    auto varTableSize = varTable.var.size();
    auto wordTableSize = varTable.word.size();
    if(varTableSize != wordTableSize) {
        printd("ERROR: var table and word table unequal sizes", to_string(varTableSize) + " != " + to_string(wordTableSize));
    }

    string env;
    for(size_t i = 0; i < varTableSize; ++i) {
        env.append(varTable.var.at(i) + "=" + varTable.word.at(i) + '\n');
    }
    return env;
}

void printenv_cmd() {
    cout << getEnvString();
}

void pipeenv_cmd(char* file, int append) {
    string text = getEnvString();
    write_to_file(file, text.c_str(), text.length(), (bool)append);
}

/***************************** Alias *****************************/

void setalias_cmd(char* name, char* word) {
    // if attempting to self-alias
    if(strcmp(name, word) == 0) {
        cout << "error: expansion of " << name << " would create a loop." << endl;
        return;
    }

    // search for any circular references to self among aliases
    for(string alias(word); existsInTable(aliasTable.name, alias);) {
        auto index = getTableIndex(aliasTable.name, alias);
        alias = aliasTable.word.at(index);

        if(alias == name) {
            cout << "error: expansion of " << name << " would create a loop." << endl;
            return;
        }
    }

    // the alias already exists and should be redefined
    if(existsInTable(aliasTable.name, name)) {
        auto index = getTableIndex(aliasTable.name, name);
        aliasTable.word.at(index) = string(word);
        return;
    }

    // the alias is new and should be appended to the table
    setAlias(name, word);

    printd("New Alias Name> ", name);
    printd("New Alias Val> ", word);
}

void unalias_cmd(char* name) {
    if(!existsInTable(aliasTable.name, name)) {
        cout << "error: alias " << name << " does not exist in table";
        return;
    }

    auto index = getTableIndex(aliasTable.name, name);
    removeTableIndex(aliasTable.name, index);
    removeTableIndex(aliasTable.word, index);
}

string getAliasString() {
    auto nameTableSize = aliasTable.name.size();
    auto wordTableSize = aliasTable.word.size();
    if(nameTableSize != wordTableSize) {
        printd("ERROR: name table and word table unequal sizes", to_string(nameTableSize) + " != " + to_string(wordTableSize));
    }

    string str;
    for(size_t i = 0; i < nameTableSize; ++i) {
        str.append(aliasTable.name.at(i) + "=" + aliasTable.word.at(i) + "\n");
    }
    return str;
}

void printalias_cmd() {
    cout << getAliasString();
}

void pipealias_cmd(char* file, int append) {
    string text = getAliasString();
    write_to_file(file, text.c_str(), text.length(), (bool)append);
}

/************************* Other Command *************************/

void unknown_command() {
    cout << "error: unknown command" << endl;
}

void invalid_alias() {
    cout << "error: invalid alias" << endl;
}

void invalid_arguments() {
    cout << "error: invalid arguments" << endl;
}

void handle_cmd(
                const char* command,
                const char* arguments,  
                const char* standardin,
                const char* stdandardout,
                int append_n_create,
                const char* stdandarderr,
                int stdout_n_file,
                int background
) {
    string cmdString;
    string argsString(arguments == NULL ? "" : arguments);

    istringstream iss;

    // break up the args in quotes and search for wildcards
    bool foundCmd = false;
    string tempCmdString(command);
    iss = istringstream(tempCmdString);
    string token;
    string quotedArgs;
    while(getline(iss, token, ' ')) {
        if(foundCmd) {
            quotedArgs.append((isPattern((char*)token.c_str()) ? subPattern(token.c_str()) : token) + " ");
        } else {
            cmdString = token;
            foundCmd = true;
        }
    }
    
    argsString.insert(0, quotedArgs);
    if(argsString.back() == ' ') {
        argsString.pop_back();
    }

    // search for any additional aliases
    iss = istringstream(argsString);
    while(getline(iss, token, ' ')) {
        if(isAlias((char*)token.c_str())) {
            auto pos = argsString.find(token);
            auto len = token.length();
            argsString.replace(pos, len, subAlias((char*)token.c_str()));
        }
    }

    // search for pipes
    iss = istringstream(cmdString + " " + argsString);
    list<vector<string>> argsList;
    while(getline(iss, token, '|')) {
        vector<string> argsArr;
        if(token.front() == ' ') {
            token.erase(token.begin());
        }
        if(token.back() == ' ') {
            token.erase(token.end()-1);
        }
        
        istringstream issNested(token);
        string tokenNested;
        while(getline(issNested, tokenNested, ' ')) {
            argsArr.push_back(tokenNested);
        }

        argsList.push_back(argsArr);
    }

#ifdef DEBUG_NUTSHELL
    cout << "ARGS LIST=";
    for(const auto& x : argsList) {
        cout << "[";
        for(const auto& y : x) {
            cout << "<" << y << ">";
        }
        cout << "] ";
    }
    cout << endl;
    cout << "INFILE=[" << (standardin == NULL ? "NULL" : standardin) << "] ";
    cout << "OUTFILE=[" << (stdandardout == NULL ? "NULL" : stdandardout) << "] ";
    cout << "ERRFILE=[" << (stdandarderr == NULL ? "NULL" : stdandarderr) << "]" << endl;
    cout << "BACKGROUND=[" << background << "]" << endl;
#endif // DEBUG_NUTSHELL

    command_t cmdTable;
    cmdTable.fileStdIn = standardin;
    cmdTable.fileStdOut = stdandardout;
    cmdTable.fileStdErr = stdandarderr;
    cmdTable.inFlag = (int)(standardin != NULL);
    cmdTable.outFlag = (int)(stdandardout != NULL ? (append_n_create+1) : 0);
    cmdTable.errFlag = (int)(stdout_n_file ? 2 : stdandarderr != NULL);
    cmdTable.background = (bool)background;
    cmdTable.args = list<vector<char*>>();

    for(const auto& v : argsList) {
        vector<char*> tempVect;
        tempVect.reserve(v.size());

        for(const auto& s : v) {
            tempVect.push_back(const_cast<char*>(s.c_str()));
        }
        tempVect.push_back((char*)NULL);

        cmdTable.args.push_back(tempVect);
    }

    int execErr = executeCommand(cmdTable);
    switch(execErr) {
        case 1: cout << "Error executing command" << endl; break;
        case 2: cout << "Error: command cannot be run" << endl; break;
    }

    restart();
}
