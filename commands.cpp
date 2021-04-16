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
#ifdef DEBUG_NUTSHELL
    cout << "CMD=[" << (command == NULL ? "NULL" : command) << "] ";
    cout << "ARGS=[" << (arguments == NULL ? "NULL" : arguments) << "] ";
    cout << "INFILE=[" << (standardin == NULL ? "NULL" : standardin) << "] ";
    cout << "OUTFILE=[" << (stdandardout == NULL ? "NULL" : stdandardout) << "] ";
    cout << "ERRFILE=[" << (stdandarderr == NULL ? "NULL" : stdandarderr) << "] ";
    cout << "BACKGROUND=[" << background << "] ";
    cout << endl;
#endif // DEBUG_NUTSHELL

    cmdTable_t cmd = {
        .command = command,
        .args = arguments,
        .standardin = standardin,
        .standardout = stdandardout,
        .standarderr = stdandarderr,
        .background = (bool)background,
        .inFlag = (int)(standardin != NULL),
        .outFlag = (int)(stdandardout != NULL ? (append_n_create+1) : 0),
        .errFlag = (int)(stdout_n_file ? 2 : stdandarderr != NULL)
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
        if(isPattern((char*)token.c_str())) {
            string wildcardList = string(subPattern(token.c_str()));

            istringstream issNested(wildcardList);
            string tokenNested;
            int additionalArgs = -1;
            while(getline(issNested, tokenNested, ' ')) {
                cmdVector.emplace_back(tokenNested);
                ++additionalArgs;
            }
            numQuotedArgs += additionalArgs;
        } else {
            cmdVector.emplace_back(token);
        }
    }

    char cmdCopy[64];
    char argsCopy[512];

    size_t argsCopyIndex = 0;

    if(numQuotedArgs > 0) {
        strcpy(cmdCopy, cmdVector.at(0).c_str());

        cmdVector.erase(cmdVector.begin());

        for(const auto& arg : cmdVector) {
            for(char c : arg) {
                argsCopy[argsCopyIndex++] = c;
            }
            argsCopy[argsCopyIndex++] = '\0';
        }
    } else {
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
        args[i+1] = &argsCopy[pos];
        pos += cmdVector.at(i).length() + 1;
    }

    if(hasArgs) {
        auto argsStrLen = strlen(cmd.args);

        for(size_t i = 0; i < argsStrLen; ++i) {
            char original = cmd.args[i];
            argsCopy[argsCopyIndex++] = original == ' ' ? '\0' : original;
        }
        argsCopy[argsCopyIndex] = '\0';
  
        pos = 0;
        for(int i = 0; i < numArgs + numQuotedArgs; ++i) {
            args[i+1] = &argsCopy[pos];
            pos = argumentsString.find(' ', pos+1) + 1;
        }
    }

#ifdef DEBUG_NUTSHELL
    for(size_t i = 0; i < argsSize; ++i) {
        cout << i << " = [" << (args[i] == (char*)NULL ? "NULL" : args[i]) << "]" << endl;
    }
#endif // DEBUG_NUTSHELL

    executeCommand(args, cmd.standardin, cmd.inFlag, cmd.standardout, cmd.outFlag, cmd.standarderr, cmd.errFlag, cmd.background);

    // int status = run_cmd(args);

    // if(status == -1) { unknown_command(); }

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