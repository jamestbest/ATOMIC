//
// Created by jamescoward on 06/11/2023.
//

#include "CLI.h"

char* ATOM_VR__CLI_ENTRY_POINT = NULL;
char* ATOM_VR__CLI_OUTPUT_NAME = ATOM_CT__CLI_DEFAULT_OUT;

charp_vec ATOM_VR__CLI_FILES;

/* main entry point for the compiler
 *
 * exit codes
 *  00 - success
 *  01 - ARGERR
 *  12 - ENOMEM
 */
int main(int argc, char** argv) {
    puts("Welcome to the ATOMIC CLI!");

    ATOM_VR__CLI_FILES = charp_vec_create(ATOM_CT__CLI_DEFAULT_FILE_BUFF_SIZE);

    if (!verify_args(argc, argv)) {
        return 1;
    }

    parse_args(argc, argv);

    print_flags();

    printf("Entry point: %s\n"
           "Output format: %s\n", ATOM_VR__CLI_ENTRY_POINT, ATOM_VR__CLI_OUTPUT_NAME);

    putchar('\n');
    for (uint i = 0; i < ATOM_VR__CLI_FILES.pos; i++) {
        char* filename = charp_vec_get(&ATOM_VR__CLI_FILES, i);

        printf("File: %s\n", filename);
    }

    CompileRet ret = compile(ATOM_VR__CLI_ENTRY_POINT, ATOM_VR__CLI_OUTPUT_NAME, ATOM_VR__CLI_FILES);

    putchar('\n');
    parse_compile_ret(ret);

    return ret.code;
}

void parse_compile_ret(CompileRet ret) {
    switch (ret.code) {
        case SUCCESS:
            printf(ATOM_CT__CLI_SUCC);
            break;
        case ERR_NO_SUCH_FILE:
            printf(ATOM_CT__CLI_ERR_NO_SUCH_FILE, ret.info);
            break;
        default:
            printf(ATOM_CT__CLI_ERR_UNKNOWN, ret.code);
            break;
    }
}

bool verify_args(int argc, char** argv) {
    if (argc <= 1) {
        PError(ATOM_CT__CLI_ERR_ARG_NONE);
        return false;
    }

    if (argv[1] == 0) {
        PError(ATOM_CT__CLI_ERR_ARG_NULL);
        return false;
    }

    return true;
}

/*
 * Take the arguments and parse into flags and input/output files
 *  .atm and .atom files are listed as inputs
 */
void parse_args(int argc, char** argv) {
    //First arg is the program name and so can be ignored

    for (int i = 1; i < argc; i++) {
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
                parse_flag(arg); //--flagSet  --!flagset
            } else {
                parse_option(arg, argv, argc, &i); //-o optionarg optionarg   e.g. -o tok ast  --this can map to flags.  -outname main.out --changes output name
            }
        } else {
            parse_file(arg); // inputfile.atom/inputfile.atm
        }
    }
}

void parse_file(char* file) {
    charp_vec_add(&ATOM_VR__CLI_FILES, file);
}

void parse_option_entry(charp_vec* args) {
    if (args->pos != 1) PWarn(ATOM_CT__CLI_WRN_OPT_ARG_COUNT, ATOM_CT__OPTION_E_STR);

    ATOM_VR__CLI_ENTRY_POINT = charp_vec_get(args, 0);
}

void parse_option_output(charp_vec* args) {
    if (args->pos != 1) PWarn(ATOM_CT__CLI_WRN_OPT_ARG_COUNT, ATOM_CT__OPTION_O_STR);

    ATOM_VR__CLI_OUTPUT_NAME = charp_vec_get(args, 0);
}

void parse_option_out(charp_vec* args) {
    //for each arg get the string arg + "-out", then hash and set the flag

    for (uint i = 0; i < args->pos; i++) {
        char* arg = args->arr[i];

        llint hash = flag_split_to_int(arg, "-"ATOM_CT__OPTION_OUT_STR);

        int index = flag_int_to_index(hash);

        if (index == -1) {
            PWarn(ATOM_CT__CLI_WRN_OPT_ARG_INVLD, ATOM_CT__OPTION_OUT_STR, arg);
            continue;
        }

        flag_set_from_idx(index, true);
    }
}

/*
 * Parse the option and use the argv values as arguments
 */
void parse_option(char *arg, char **argv, int argc, int *i) {
    llint option_hash = flag_to_int(&arg[1]);

    charp_vec args = get_option_args(argv, i, argc);

    if (args.pos == 0) {
        PError(ATOM_CT__CLI_ERR_OPT_ARG, arg);
        exit(ARGERR);
    }

    switch (option_hash) {
        case ATOM_CT__OPTION_E_HASH:
            parse_option_entry(&args);
            break;
        case ATOM_CT__OPTION_O_HASH:
            parse_option_output(&args);
            break;
        case ATOM_CT__OPTION_OUT_HASH:
            parse_option_out(&args);
            break;
        default:
            printf("Error: Unknown option \"%s\"", arg);
            break;
    }

    charp_vec_destroy(&args);
}

charp_vec get_option_args(char** argv, int* argp, int argc) {
    charp_vec args = charp_vec_create(4);

    while (*argp + 1 < argc && argv[*argp + 1][0] != '-') {
        charp_vec_add(&args, argv[*argp + 1]);
        (*argp)++;
    }

    return args;
}

void parse_flag(char* arg) {
    //turn the flag into an int from past --
    //this should be based on the next 8 characters of the flag
    //e.g. --!AST-OUT
    bool enabled = true;
    uint start = 2;
    if (arg[2] == '!') {
        start = 3;
        enabled = false;
    }

    bool ret = flag_set(&arg[start], enabled);

    if (!ret) {
        PWarn(ATOM_CT__CLI_WRN_OPT_FLG_INVLD, &arg[start]);
        return;
    }
}