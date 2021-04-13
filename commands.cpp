#include "commands.h"
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>

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
    exit(0);
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

    if(any_of(varTable.var.begin(), varTable.var.end(), [var](const string& s){ return s.compare(var) == 0; })) {
        auto it = find(varTable.word.begin(), varTable.word.end(), word);
        
        if(it != varTable.word.end()) {
            /* if both the variable and the word already exist */
            return;
        }

        /* redefine the variable if variable exists */
        varTable.word.emplace(it, word);
        return;
    }

    /* else the variable is new, and must be added to the table */
    setVar(var, word);

    printd("New Variable Name> ", var);
    printd("New Variable Val> ", word);
}

void unsetenv_cmd(char* var) {
    auto index = getTableIndex(varTable.var, var);
    removeTableIndex(varTable.var, index);
    removeTableIndex(varTable.word, index);
}

void printenv_cmd() {
    auto varTableSize = varTable.var.size();
    auto wordTableSize = varTable.word.size();
    if(varTableSize != wordTableSize) {
        printd("ERROR: var table and word table unequal sizes", to_string(varTableSize) + " != " + to_string(wordTableSize));
    }

    for(size_t i = 0; i < varTableSize; ++i) {
        cout << varTable.var.at(i) << " = " << varTable.word.at(i) << endl;
    }
}

/***************************** Alias *****************************/

void setalias_cmd(char* name, char* word) {
    if(strcmp(name, word) == 0) {
        /* if alias == word */
        cout << "error: expansion of " << name << " would create a loop." << endl;
        return;
    }

    string alias(word);
    while(true) {
        if(!existsInTable(aliasTable.name, alias)) {
            break;
        }

        auto index = getTableIndex(aliasTable.name, alias);
        alias = aliasTable.word.at(index);

        if(alias == name) {
            cout << "error: expansion of " << name << " would create a loop." << endl;
            return;
        }
    }

    if(existsInTable(aliasTable.name, name)) {
        auto it = find(aliasTable.word.begin(), aliasTable.word.end(), word);
        
        if(it != aliasTable.word.end()) {
            /* if both the alias and the word already exist */
            return;
        }

        /* redefine the alias if alias exists */
        aliasTable.word.emplace(it, word);
        return;
    }

    /* else the variable is new, and must be added to the table */
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

void printalias_cmd() {
    printd("running cmd:", "printalias_cmd()");

    auto nameTableSize = aliasTable.name.size();
    auto wordTableSize = aliasTable.word.size();
    if(nameTableSize != wordTableSize) {
        printd("ERROR: name table and word table unequal sizes", to_string(nameTableSize) + " != " + to_string(wordTableSize));
    }

    for(size_t i = 0; i < nameTableSize; ++i) {
        cout << aliasTable.name.at(i) << "=" << aliasTable.word.at(i) << endl;
    }
}

/************************* Other Command *************************/

void unknown_command() {
    cout << "error: unknown command" << endl;
}

void handle_cmd(const char* command, 
                const char* options, 
                const char* args,  
                const char* standardin,
                const char* stdandardout,
                const char* stdandarderr,
                int background
) {
    cmdTable_t cmd = {
        .command = command,
        .options = options,
        .args = args,
        .standardin = standardin,
        .standardout = stdandardout,
        .standarderr = stdandarderr,
        .background = background
    };

    interpret_cmd(cmd);
}

void interpret_cmd(const cmdTable_t& cmd) {
    string command(cmd.command);
    command.append(" 2> /dev/null"); // suppress stderr
    
    int status = system(command.c_str());
    
    if (status < 0) {
        printerr();
        return;
    } else if (status == 0x7F00) {
        unknown_command();
        return;
    }

    // printd("CMD:", cmd.command);
    // printd("ARGS:", cmd.args);
}