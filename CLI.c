//
// Created by jamescoward on 06/11/2023.
//

#include "CLI.h"

#include "SharedIncludes/Helper_File.h"
#include "SharedIncludes/Flag_shared.h"

#include <errno.h>
#include <Messages.h>
#include <stdlib.h>
#include <string.h>

char* ATOM_VR__CLI_ENTRY_POINT = NULL;
char* ATOM_VR__CLI_OUTPUT_NAME = ATOM_CT__CLI_DEFAULT_OUT;

Vector ATOM_VR__CLI_FILES;

/* main entry point for the compiler
 *
 * exit codes
 *  00 - success
 *  01 - ARGERR
 *  12 - ENOMEM
 */
int main(int argc, char** argv) {
    puts("Welcome to the ATOMIC CLI!");

    ATOM_VR__CLI_FILES = vector_create(ATOM_CT__CLI_DEFAULT_FILE_BUFF_SIZE);

    if (!verify_args(argc, argv)) {
        return 1;
    }

    parse_args(argc, argv);

    if (flag_get_value(ATOM_CT__FLAG_FLAGS_OUT))
        print_flags();

    printf("Entry point: %s\n"
           "Output format: %s\n", ATOM_VR__CLI_ENTRY_POINT, ATOM_VR__CLI_OUTPUT_NAME);

    putchar('\n');
    for (uint i = 0; i < ATOM_VR__CLI_FILES.pos; i++) {
        char* filename = vector_get_unsafe(&ATOM_VR__CLI_FILES, i);

        printf("File: %s\n", filename);
    }

    char* cwd = get_dir(argv[0]); //get the c_char working directory

    const CompileRet ret = compile(ATOM_VR__CLI_ENTRY_POINT, ATOM_VR__CLI_OUTPUT_NAME, cwd, ATOM_VR__CLI_FILES);

    free(cwd);

    putchar('\n');
    parse_compile_ret(ret);

    vector_destroy(&ATOM_VR__CLI_FILES);

    hadron_verify();
    hadron_cleanup();

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
        case LEXERR:
            printf(ATOM_CT__CLI_ERR_LEXERR, ret.code); // - BASE_ERR?
            break;
        default:
            printf(ATOM_CT__CLI_ERR_UNKNOWN, ret.code);
            break;
    }
}

bool verify_args(int argc, char** argv) {
    if (argc <= 1) {
        PError(ATOM_CT__CLI_ERR_ARG_NONE, argc);
        return false;
    }

    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL) {
            PError(ATOM_CT__CLI_ERR_ARG_NULL, i);
            return false;
        }
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
                parse_option(&arg[1], argv, argc, &i); //-o optionarg optionarg   e.g. -o tok ast  --this can map to flags.  -outname main.out --changes output name
            }
        } else {
            parse_file(arg); // inputfile.atom/inputfile.atm
        }
    }
}

void parse_file(char* file) {
    vector_add(&ATOM_VR__CLI_FILES, file);
}

void parse_option_entry(Vector* args) {
    if (args->pos != 1) PWarn(ATOM_CT__CLI_WRN_OPT_ARG_COUNT,  flag_get_str(ATOM_CT__OPTION_E));

    ATOM_VR__CLI_ENTRY_POINT = vector_get_unsafe(args, 0);
}

void parse_option_output(Vector* args) {
    if (args->pos != 1) PWarn(ATOM_CT__CLI_WRN_OPT_ARG_COUNT, flag_get_str(ATOM_CT__OPTION_O));

    ATOM_VR__CLI_OUTPUT_NAME = vector_get_unsafe(args, 0);
}

void parse_option_hadron(const Vector* args) {
    for (uint i = 0; i < args->pos; ++i) {
        const char* arg = args->arr[i];
    }
}

void parse_option_debuglvl(Array args) {
    assert(false);
}

void parse_option_out(Array args) {

}

uint verify_numeric_errno(const char* arg, const StaticOptionArgInfo* arg_info) {
    if (errno != 0) {
        if (errno == ERANGE) {
            error(ATOM_CT__CLI_ERR_ARG_VALUE_OUT_OF_RANGE,
                ATOM_FP__TYPES_STR[arg_info->type],
                arg
            );
        } else {
            error(ATOM_CT__CLI_ERR_ARG_VALUE_UNKNOWN_ERR,
                arg,
                ATOM_FP__TYPES_STR[arg_info->type]
            );
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

struct VerifiedArgs {
    uint errcode;
    Array translated_args;
} verify_option_arguments(const Vector args, const enum ATOM_CT__OPTIONS option) {
    const StaticOptionInfo* info = &ATOM_CT__OPTIONINFO[option];
    Array ret_arr = arr_construct(sizeof (OptionData), args.pos);

    for (uint i = 0; i < args.pos; ++i) {
        const StaticOptionArgInfo* arg_info = &info->args[i];
        const char* arg = args.arr[i];

        switch (arg_info->type) {
            case FP_TYPE_CHARACTER: {
                if (strlen(arg) != 1) {
                    goto verify_option_arguments_invalid_type;
                }
                arr_add_dyn(&ret_arr, &arg[0], sizeof (arg[0]));
                break;
            }
            case FP_TYPE_INTEGER: {
                char* end;
                long long val = strtoll(arg, &end, 10);

                if (verify_numeric_errno(arg, arg_info) != EXIT_SUCCESS) {
                    goto verify_option_arguments_return_error;
                }

                if (end != &arg[strlen(arg)]) {
                    goto verify_option_arguments_invalid_type;
                }

                arr_add_dyn(&ret_arr, &val, sizeof (val));
                break;
            }
            case FP_TYPE_NATURAL: {
                char* end;
                unsigned long long val = strtoull(arg, &end, 10);

                if (verify_numeric_errno(arg, arg_info) != EXIT_SUCCESS) {
                    goto verify_option_arguments_return_error;
                }

                if (end != &arg[strlen(arg)]) {
                    goto verify_option_arguments_invalid_type;
                }

                arr_add_dyn(&ret_arr, &val, sizeof (val));
                break;
            }
            case FP_TYPE_STR: {
                arr_add_dyn(&ret_arr, &arg, sizeof (arg));
                break;
            }
            case FP_TYPES_COUNT:
                assert(false);
        }

        continue;

    verify_option_arguments_invalid_type:
        error(ATOM_CT__CLI_ERR_ARG_VALUE_INVALID,
            info->option_name,
            arg_info->arg_name,
            ATOM_FP__TYPES_STR[arg_info->type],
            arg
        );
        goto verify_option_arguments_return_error;
    }

verify_option_arguments_return_error:
    return (struct VerifiedArgs){.errcode = EXIT_FAILURE, .translated_args = ARRAY_ERR};
}

/*
 * Parse the option and use the argv values as arguments
 */
void parse_option(char *option_name, char **cli_args, int cli_arg_count, int *option_name_index_in_args) {
    Vector args = get_option_args(cli_args, option_name_index_in_args, cli_arg_count);

    if (args.pos == 0) {
        PError(ATOM_CT__CLI_ERR_OPT_ARG, option_name);
        exit(ARGERR);
    }

    const enum ATOM_CT__OPTIONS option_index = option_find(option_name);

    if (option_index == (enum ATOM_CT__OPTIONS)-1) {
        PWarn(ATOM_CT__CLI_WRN_OPT_INVALID, option_name);
        return;
    }

    const struct VerifiedArgs res = verify_option_arguments(args, option_index);
    vector_destroy(&args);

    if (res.errcode != EXIT_SUCCESS) {
        exit(ARGERR);
    }

    const Array args_t = res.translated_args;
    switch (option_index) {
        case ATOM_CT__OPTION_DEBUGLVL:
            parse_option_debuglvl(args_t);
            break;
        case ATOM_CT__OPTION_E:
            break;
        case ATOM_CT__OPTION_O:
            break;
        case ATOM_CT__OPTION_OUT:
            break;
        case ATOM_CT__OPTION_TEST:
            break;
        case ATOM_CT__OPTION_COUNT:
            break;
    }

    vector_destroy(&args);
}

Vector get_option_args(char** argv, int* argp, int argc) {
    Vector args = vector_create(ATOM_CT__CLI_DEFAULT_OPTION_BUFF_SIZE);

    while (*argp + 1 < argc && argv[*argp + 1][0] != '-') {
        vector_add(&args, argv[*argp + 1]);
        (*argp)++;
    }

    return args;
}

void parse_flag(char* arg) {
    //turn the flag into an int from past --
    //e.g. --!AST-OUT
    bool enabled = true;
    uint start = 2;
    if (arg[2] == '!') {
        start++;
        enabled = false;
    }

    const size_t flag_index = flag_find(&arg[start]);

    if (flag_index == (size_t)-1) {
        PWarn(ATOM_CT__CLI_WRN_FLAG_INVALID, &arg[start]);
        return;
    }

    flag_set(flag_index, enabled);
}
