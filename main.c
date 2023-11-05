#include "main.h"

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
}


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

/*
 * Take the arguments and parse into flags and input/output files
 *  .atm and .atom files are listed as inputs
 */
void parseArgs(int argc, char** argv) {
    //First arg is the program name and so can be ignored

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
                parseFlag(arg, arg_len); //--flagSet  --!flagset
            } else {
//                parseOption(arg, arg_len, argv, i); //-o optionarg optionarg   e.g. -o tok ast  --this can map to flags.  -outname main.out --changes output name
            }
        } else {
//            parseFile(arg, arg_len); // inputfile.atom/inputfile.atm
        }
    }
}

void parseFlag(char* arg, uint arg_len) {
    //turn the flag into an int from past --
    //this should be based on the next 8 characters of the flag
    //e.g. --!AST-OUT
    bool enabled = true;
    uint start = 2;
    if (arg[2] == '!') {
        start = 3;
        enabled = false;
    }

    int index = ATOM__FLAG_TO_INDEX(&arg[start]);

    if (index == -1) {
        assert(false);//just for when making the flags to check for missed ones
    }

    ATOM__FLAGS[index] = enabled;
}