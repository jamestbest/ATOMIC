#include <stdio.h>
#include "Lexer.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool verifyArgs(int argc, char** argv) {
    if (argc <= 1) {
        puts("The ATOMIC compiler requires at least one argument (file to compile)");
        return false;
    }

    if (argv[1] == 0) {
        puts("Error with argv, null pointer in first argument (bar program name)");
        return false;
    }

    return true;
}

unsigned int len(const char* string) {
    int i = 0;
    while (string[i] != '\0') {i++;};
    return i;
}

bool streq(const char* stra, const char* strb) {
    int i = 0;
    while (stra[i] != '\0' && strb[i] != '\0') {
        if (stra[i] != strb[i]) return false;
        i++;
    }
    
    return stra[i] == strb[i]; //have both strings ended?
}

bool startswith(char* pattern, char* string) {
    //does the string start with pattern

    uint lp = len(pattern);
    uint ls = len(string);

    if (lp > ls) return false;
    if (lp == ls) return streq(string, pattern);

    //pattern : abcde
    //string  : abcdefghi
    //string  : abcde0ghi

    char save = string[lp];
    string[lp] = '\0';

    bool ret = streq(pattern, string);
    string[lp] = save;

    return ret;
}

/*
 * Take the arguments and parse into flags and input/output files
 *  .atm and .atom files are listed as inputs
 */
void parseArgs(int argc, char** argv) {
    //First arg is the program name and so can be ignore

    for (int i = 0; i < argc; i++) {
        char* arg = argv[i];

        if (arg == 0) {
            return;
        }

        uint arg_len = len(arg);

        if (arg_len < 1) {
            continue;
        }

        //length is at least one
        if (arg[0] == '-') {
            if (arg_len >= 2 && arg[1] == '-') {
                parseFlag(arg, arg_len); //--flagSet
            } else {
                parseOption(arg, arg_len); //-o optionarg optionarg
            }
        } else {
            parseFile(arg, arg_len); // inputfile.atom/inputfile.atm
        }
    }
}

/* main entry point for the compiler
 *
 * exit codes
 *  0 - success
 *  1 - error with arguments
 */
int main(int argc, char** argv) {
    puts("Welcome to the ATOMIC Compiler!");

    if (!verifyArgs(argc, argv)) {
        return 1;
    }

    parseArgs(argc, argv);

    printf("Hello, World!\n");
    int a = 12;
    return test(a);
}
