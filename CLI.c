//
// Created by jamescoward on 06/11/2023.
//

#include "CLI.h"

#include "SharedIncludes/Helper_File.h"
#include "SharedIncludes/Flag_shared.h"
#include "SharedIncludes/Messages.h"

#include <errno.h>
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

    for (uint i = 1; i < argc; i++) {
        char* arg = argv[i];

        if (arg == 0) {
            return;
        }

        const uint arg_len = len(arg);

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

void parse_option_hadron(const Vector* args) {
    for (uint i = 0; i < args->pos; ++i) {
        const char* arg = args->arr[i];
    }
}

void parse_option_debuglvl(Array args) {
    assert(false);
}

void parse_option_out(const Array args) {
    for (uint i = 0; i < args.pos; ++i) {
        const ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_ENUM arg = *(ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_ENUM*)arr_ptr(&args, i);

        enum ATOM_CT__FLAGS flag;
        switch (arg) {
            case ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_AST:
                flag = ATOM_CT__FLAG_AST_OUT;
                break;
            case ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_FLAGS:
                flag = ATOM_CT__FLAG_FLAGS_OUT;
                break;
            case ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_SCOPE:
                flag = ATOM_CT__FLAG_SCOPE_OUT;
                break;
            case ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_TOK:
                flag = ATOM_CT__FLAG_TOK_OUT;
                break;
            case ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_VLTOK:
                flag = ATOM_CT__FLAG_VLTOK_OUT;
                break;
            case ATOM_CT__OPTION_OUT_ARG_OUTPUT_VALID_VTOK:
                flag = ATOM_CT__FLAG_VTOK_OUT;
                break;
            default:
                assert(false);
        }
        flag_set(flag, true);
    }
}

void parse_option_o(const Array args) {
    ATOM_VR__CLI_OUTPUT_NAME = *(char**)arr_ptr(&args, ATOM_CT__OPTION_O_ARG_OUTPUT_FORMAT);
}

void parse_option_entry(const Array args) {
    ATOM_VR__CLI_ENTRY_POINT = *(char**)arr_ptr(&args, ATOM_CT__OPTION_E_ARG_Entry);
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

int compare_ints(const void* a, const void* b) {
    const long long av = *(long long*)a;
    const long long bv = *(long long*)b;

    if (av == bv) return 0;
    if (av < bv) return -1;
    return 1;
}

int compare_naturals(const void* a, const void* b) {
    const unsigned long long av = *(unsigned long long*)a;
    const unsigned long long bv = *(unsigned long long*)b;

    if (av == bv) return 0;
    if (av < bv) return -1;
    return 1;
}

int compare_characters(const void* a, const void* b) {
    const char av = *(char*)a;
    const char bv = *(char*)b;

    return av - bv;
}

int compare_strings(const void* a, const void* b) {
    return strcasecmp(*(char**)a, *(char**)b);
}

int (*cmp_funcs[])(const void*, const void*) = {
    [FP_TYPE_STR] = compare_strings,
    [FP_TYPE_INTEGER] = compare_ints,
    [FP_TYPE_NATURAL] = compare_naturals,
    [FP_TYPE_CHARACTER] = compare_characters,
};

struct ArgInListRes {
    uint errcode;
    size_t enum_pos;
} verify_arg_in_list(void* arg, const StaticOptionArgInfo* arg_info) {
    if (arg_info->arg_option_count == 0) {
        return (struct ArgInListRes){.errcode= EXIT_SUCCESS, .enum_pos = (size_t)-1};
    }

    const OptionData* res = bsearch(arg, arg_info->data, arg_info->arg_option_count, sizeof (*arg_info->data), cmp_funcs[arg_info->type]);

    if (!res) {
        return (struct ArgInListRes){.errcode= EXIT_FAILURE, .enum_pos = (size_t)-1};;
    }
    return (struct ArgInListRes){.errcode= EXIT_SUCCESS, .enum_pos = res - arg_info->data};
}

void print_valid_arg_options(const StaticOptionArgInfo* arg_info) {
    for (uint i = 0; i < arg_info->arg_option_count; ++i) {
        const OptionData* data = &arg_info->data[i];
        printf("  %d: ", i + 1);
        switch (arg_info->type) {
            case FP_TYPE_STR:
                printf("%s", data->str);
                break;
            case FP_TYPE_INTEGER:
                printf("%lld", data->integer);
                break;
            case FP_TYPE_NATURAL:
                printf("%llu", data->natural);
                break;
            case FP_TYPE_CHARACTER:
                printf("%c", data->character);
                break;
            default:
                assert(false);
        }
        newline();
    }
}

struct VerifiedArgs {
    uint errcode;
    Array translated_args;
} verify_option_arguments(const Vector args, const enum ATOM_CT__OPTIONS option) {
    const StaticOptionInfo* info = &ATOM_CT__OPTIONINFO[option];
    Array ret_arr = arr_construct(sizeof (OptionData), args.pos);

    uint arg_info_pos = 0;
    for (uint i = 0; i < args.pos; ++i) {
        const StaticOptionArgInfo* arg_info = &info->args[arg_info_pos];

        // if the next arg info is valid then move on to it
        if (arg_info_pos >= info->arg_info_count) {
            // if the next arg info isn't valid then we need the last one to have been a repeatable arg, else error
            if (!arg_info->repeated) {
                error("Option `%s` given more arguments than allowed. Expected %zu args\n",
                    info->option_name,
                    info->arg_info_count
                );
                goto verify_option_arguments_return_error;
            }
        } else if (arg_info_pos + 1 < info->arg_info_count) {
            arg_info_pos++;
        }

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

        const struct ArgInListRes arg_verified = verify_arg_in_list(arr_ptr(&ret_arr, ret_arr.pos - 1), arg_info);

        if (arg_verified.errcode == EXIT_SUCCESS) {
            if (arg_verified.enum_pos != (size_t)-1) {
                // if we are to choose from a number of options then give the index not the value
                arr_set_dyn(&ret_arr, ret_arr.pos - 1, &arg_verified.enum_pos, sizeof (arg_verified.enum_pos));
            }
            continue;
        }

    verify_option_arguments_invalid_arg:
        error("Argument `%s` given to option `%s` is not in list of valid options:\n", arg, info->option_name);
        print_valid_arg_options(arg_info);
        goto verify_option_arguments_return_error;

    verify_option_arguments_invalid_type:
        error(ATOM_CT__CLI_ERR_ARG_VALUE_INVALID,
            info->option_name,
            arg_info->arg_name,
            ATOM_FP__TYPES_STR[arg_info->type],
            arg
        );
        goto verify_option_arguments_return_error;
    }

    return (struct VerifiedArgs){.errcode = EXIT_SUCCESS, .translated_args = ret_arr};

verify_option_arguments_return_error:
    arr_destroy(&ret_arr);
    return (struct VerifiedArgs){.errcode = EXIT_FAILURE, .translated_args = ARRAY_ERR};
}

/*
 * Parse the option and use the argv values as arguments
 */
void parse_option(char* option_name, char** cli_args, int cli_arg_count,
                  uint* option_name_index_in_args) {
    Vector args = get_option_args(cli_args, option_name_index_in_args, cli_arg_count);

    if (args.pos == 0) {
        PError(ATOM_CT__CLI_ERR_OPT_ARG, option_name);
        exit(ARGERR);
    }

    const enum ATOM_CT__OPTIONS option_index = option_find(option_name);

    if (option_index == (enum ATOM_CT__OPTIONS)-1) {
        PError(ATOM_CT__CLI_ERR_OPT_INVALID, option_name);
        exit(ARGERR);
    }

    const struct VerifiedArgs res = verify_option_arguments(args, option_index);
    vector_destroy(&args);

    if (res.errcode != EXIT_SUCCESS) {
        exit(ARGERR);
    }

    Array args_t = res.translated_args;
    switch (option_index) {
        case ATOM_CT__OPTION_DEBUGLVL:
            parse_option_debuglvl(args_t);
            break;
        case ATOM_CT__OPTION_E:
            parse_option_entry(args_t);
            break;
        case ATOM_CT__OPTION_O:
            parse_option_o(args_t);
            break;
        case ATOM_CT__OPTION_OUT:
            parse_option_out(args_t);
            break;
        case ATOM_CT__OPTION_TEST:
        case ATOM_CT__OPTION_COUNT:
            assert(false);
    }
    arr_destroy(&args_t);
}

Vector get_option_args(char** argv, uint* argp, const int argc) {
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
