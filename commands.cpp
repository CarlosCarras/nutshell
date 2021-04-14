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
    cout << "DEST: " << (dest == NULL ? "NULL" : dest) << endl;
    int status = chdir(dest);
    if (status < 0) { printerr(); }
}

void pwd_cmd() {
    getcwd(cwd, sizeof(cwd));
    cout << cwd << endl;
}

void echo_cmd(char* val) {
    string str(val);
    if(str.at(0) == '$' && str.at(1) == '{' && str.at(str.length()-1) == '}') {
        // check for env var
        string envVar = str.substr(2, str.length()-3);
        if(existsInTable(varTable.var, envVar)) {
            // print out env var value
            cout << varTable.word.at(getTableIndex(varTable.var, envVar)) << endl;
            return;
        }
    }

    cout << str << endl;
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

void printenv_cmd() {
    auto varTableSize = varTable.var.size();
    auto wordTableSize = varTable.word.size();
    if(varTableSize != wordTableSize) {
        printd("ERROR: var table and word table unequal sizes", to_string(varTableSize) + " != " + to_string(wordTableSize));
    }

    for(size_t i = 0; i < varTableSize; ++i) {
        cout << varTable.var.at(i) << "=" << varTable.word.at(i) << endl;
    }
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

void invalid_alias() {
    cout << "error: invalid alias" << endl;
}

void invalid_arguments() {
    cout << "error: invalid arguments" << endl;
}

void handle_cmd(const char* command, 
                const char* options, 
                const char* arguments,  
                const char* standardin,
                const char* stdandardout,
                const char* stdandarderr,
                int background
) {

    // cout << "CMD: " << command << endl;
    // cout << "OPTIONS: " << options << endl;
    cout << "ARGS: " << (arguments == NULL ? "NULL" : arguments) << endl;

    cmdTable_t cmd = {
        .command = command,
        .options = options,
        .args = arguments,
        .standardin = standardin,
        .standardout = stdandardout,
        .standarderr = stdandarderr,
        .background = background
    };

    interpret_cmd(cmd);
    restart();
}

void interpret_cmd(const cmdTable_t& cmd) {
    // string command(cmd.command);
    // command.append(" 2> /dev/null"); // suppress stderr

    // string cmdString(cmd.command);
    // if(cmd.args != NULL && cmd.args[0] != '\0') { cmdString.append(cmd.args); };

    string commandString(cmd.command);
    ptrdiff_t numQuotedArgs = count(commandString.begin(), commandString.end(), ' ');

    istringstream iss(commandString);
    string token;
    vector<string> cmdVector;
    while(getline(iss, token, ' ')) {
        cmdVector.emplace_back(token);
    }

    char* cmdCopy;
    char* quotedArgsCopy;
    if(numQuotedArgs > 0) {
        cmdCopy = new char(cmdVector.at(0).length() + 1);
        strcpy(cmdCopy, cmdVector.at(0).c_str());

        cmdVector.erase(cmdVector.begin());

        size_t quotedArgsCopySize = 0;
        for(size_t i = 0; i < cmdVector.size(); ++i) {
            quotedArgsCopySize += cmdVector.at(i).length() + 1;
        }

        quotedArgsCopy = new char(quotedArgsCopySize);

        size_t index = 0;
        for(const auto& arg : cmdVector) {
            for(char c : arg) {
                quotedArgsCopy[index++] = c;
            }
            quotedArgsCopy[index++] = '\0';
        }
    } else {
        cmdCopy = new char(commandString.length() + 1);
        strcpy(cmdCopy, commandString.c_str());
    }

    bool hasArgs = cmd.args != NULL;

    ptrdiff_t numArgs = 0;
    string argumentsString;
    if(hasArgs) {
        argumentsString = string(cmd.args);
        numArgs = count(argumentsString.begin(), argumentsString.end(), ' ') + 1;
    }
    size_t argsSize = numQuotedArgs + numArgs + 2;

    char* args[argsSize];

    args[0] = cmdCopy;
    args[argsSize-1] = (char*)NULL;

    size_t pos = 0;
    for(int i = 0; i < numQuotedArgs; ++i) {
        args[i+1] = &quotedArgsCopy[pos];
        pos += cmdVector.at(i).length() + 1;
    }

    char* argsCopy;
    if(hasArgs) {
        auto argsStrLen = strlen(cmd.args);
        argsCopy = new char(argsStrLen+1);

        for(size_t i = 0; i < argsStrLen; ++i) {
            char original = cmd.args[i];
            argsCopy[i] = original == ' ' ? '\0' : original;
        }
        argsCopy[argsStrLen] = '\0';
  
        pos = 0;
        for(int i = 0; i < numArgs; ++i) {
            args[i+1+numQuotedArgs] = &argsCopy[pos];
            pos = argumentsString.find(' ', pos+1) + 1;
        }
    }

    for(size_t i = 0; i < argsSize; ++i) {
        cout << i << " = [" << (args[i] == (char*)NULL ? "NULL" : args[i]) << "]" << endl;
    }

    int status = run_cmd(args);

    delete[] cmdCopy;
    if(hasArgs) {
        delete[] argsCopy;
    }
    if(numQuotedArgs > 0) {
        delete[] quotedArgsCopy;
    }

    if(status == -1) { unknown_command(); }

    // auto bufferLength = strlen(cmd.command);
    // if(cmd.args != NULL) {
    //     bufferLength += strlen(cmd.args);
    // }

    // char cmdBuffer[bufferLength + 8];
    // strcpy(cmdBuffer, cmd.command);
    // if(cmd.args != NULL && cmd.args[0] != '\0') {
    //     strcat(cmdBuffer, (const char*)" ");
    //     strcat(cmdBuffer, cmd.args);
    // };

    // int status = run_cmd(cmdBuffer);

    // int status = system(command.c_str());
    
    // if      (status < 0)       { printerr();        return; }
    // else if (status == 0x7F00) { unknown_command(); return; }

    //printd("CMD:", cmd.command);
    //printd("ARGS:", cmd.args);
}