//
// Created by jamescoward on 09/11/2023.
//

#include "FlagPreprocessorInternal.h"

#include "SharedIncludes/Messages.h"

#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARGS_COUNT 3

bool verify_arguments(int argc, char** argv);

uint parse_flag_file(FILE* file);

void print_all_flaginfos(const Vector* vec);
void print_all_options(const Vector* vec);

Vector flagInfos;
Vector optionInfos;

size_t l_pos;
size_t p_pos;
Array tokens;

int compare_flag_infos(const void* a, const void* b) {
    const FlagInfo* flag_a = *(const FlagInfo**)a;
    const FlagInfo* flag_b = *(const FlagInfo**)b;

    return strcmp(flag_a->flag_name, flag_b->flag_name);
}

int compare_option_infos(const void* a, const void* b) {
    const OptionInfo* opt_a = *(const OptionInfo**)a;
    const OptionInfo* opt_b = *(const OptionInfo**)b;

    return strcmp(opt_a->option_name, opt_b->option_name);
}

int compare_arg_options_str(const void* a, const void* b) {
    return strcmp(a, b);
}

int cmp_str_to_flag(const void* a, const void* b) {
    const char* stra = a;
    const FlagInfo* flag = *(const FlagInfo**)b;

    return strcmp(stra, flag->flag_name);
}

size_t find_flag_loc(const char* flag_to_search) {
    const FlagInfo** res = bsearch(flag_to_search, flagInfos.arr, flagInfos.pos, sizeof (FlagInfo*), cmp_str_to_flag);

    if (res == NULL) return -1;
    return res - (const FlagInfo**)flagInfos.arr;
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
    return strcmp(*(char**)a, *(char**)b);
}

int (*cmp_funcs[])(const void*, const void*) = {
    [FP_TYPE_STR] = compare_strings,
    [FP_TYPE_INTEGER] = compare_ints,
    [FP_TYPE_NATURAL] = compare_naturals,
    [FP_TYPE_CHARACTER] = compare_characters,
};

void sort_all_data() {
    // sort the flags array
    qsort(flagInfos.arr, flagInfos.pos, sizeof (FlagInfo*), compare_flag_infos);
    // sort the options array
    qsort(optionInfos.arr, optionInfos.pos, sizeof(OptionInfo*), compare_option_infos);

    for (uint i = 0; i < optionInfos.pos; ++i) {
        const OptionInfo* info = vector_get_unsafe(&optionInfos, i);
        for (uint j = 0; j < info->arg_infos.pos; ++j) {
            const OptionArgInfo* args = vector_get_unsafe(&info->arg_infos, j);
            if (args->arg_options.pos != 0) {
                // if there are arg options sort them
                int (*cmp_func)(const void*, const void*) = cmp_funcs[args->type];

                qsort(args->arg_options.arr, args->arg_options.pos, args->arg_options.element_size, cmp_func);
            }
        }
    }
}

int main(const int argc, char** argv) {
    puts("Welcome to the ATOMIC Flag preprocessor");

    verify_arguments(argc, argv);

    flagInfos = vector_create(15);
    optionInfos = vector_create(10);

    const char* flag_file_name = argv[1];
    FILE* flag_file = fopen(flag_file_name, "r");

    const uint ret = parse_flag_file(flag_file);

    if (ret != EXIT_SUCCESS) {
        return error("Errors found during parsing of flag file, please fix and re-run. Errcode: %u\n", ret).code;
    }

    sort_all_data();

    print_all_flaginfos(&flagInfos);
    print_all_options(&optionInfos);

    const char* output_file_name = argv[2];

    const uint write_res = write_out_flag_data(output_file_name);

    if (write_res != EXIT_SUCCESS) {
        error("Error during writing process\n");
    }

    vector_disseminate_destruction(&flagInfos);
//    vector_disseminate_destruction(&optionInfos);
}

const char* translate_name_for_enum(const char* flag) {
    const uint len = strlen(flag);

    char* ret = malloc((len + 1) * sizeof (char));

    for (uint i = 0; i < len; ++i) {
        ret[i] = flag[i] == '-' ? '_' : flag[i];
    }
    ret[len] = '\0';

    return ret;
}

// [[todo]] change this to just be 2 functions, this is ridiculously obfuscated abstraction
void append_enum(FILE* file, const char* enum_name,
                 const char* enum_element_prefix,
                 const char* count_element_name,
                 const Vector* vector,
                 const char* (*get)(size_t index)) {
    fprintf(file, "enum %s {\n", enum_name);
    for (uint i = 0; i < vector->pos; ++i) {
        const char* name = get(i);
        const char* translated_name = translate_name_for_enum(name);
        fprintf(file, "    %s%s,\n", enum_element_prefix, translated_name);
        free((void*)translated_name);
    }
    fprintf(file, "    %s%s\n};\n\n", enum_element_prefix, count_element_name);
}

const char* get_flag_name(const size_t index) {
    return ((FlagInfo*)flagInfos.arr[index])->flag_name;
}

void append_flag_enum(FILE* header_file) {
    append_enum(header_file,
        ATOM_FP__ENUM_FLAGS_NAME,
        ATOM_FP__FLAG_START,
        ATOM_FP__FLAGS_COUNT_NAME,
        &flagInfos,
        get_flag_name
    );
}

const char* get_option_name(const size_t index) {
    return ((OptionInfo*)vector_get_unsafe(&optionInfos, index))->option_name;
}

void append_option_enum(FILE* header_file) {
    append_enum(header_file,
        ATOM_FP__ENUM_OPTIONS_NAME,
        ATOM_FP__OPTION_START,
        ATOM_FP__OPTIONS_COUNT_NAME,
        &optionInfos,
        get_option_name
    );
}

void write_header_out_preamble(FILE* header) {
    const time_t t = time(NULL);
    const struct tm *local_time = localtime(&t);
    char buff[32];
    const size_t res = strftime(buff, sizeof(buff), "%c", local_time);
    if (res == 0) {
        const char* errstr = "Error fetching date";
        memcpy(buff, errstr, strlen(errstr) + 1);
    }

    fprintf(header, "//\n"
           "// Generated by ATOM Flag Preprocessor: %s\n"
           "//\n"
           "\n"
           "#ifndef "ATOM_FP__HEADER_GUARD"\n"
           "#define "ATOM_FP__HEADER_GUARD"\n"
           "\n"
           "#include <stdbool.h>\n"
           "#include \"FlagPreprocessorInject.h\"\n"
           "#include \"Vector.h\"\n"
           "\n",
           buff
    );
}

void write_header_out_amble(FILE* header) {
    fputs("extern bool ATOM_VR__FLAGS["ATOM_FP__FLAG_START ATOM_FP__FLAGS_COUNT_NAME"];\n"
             "extern const FlagInfo ATOM_CT__FLAGINFO["ATOM_FP__FLAG_START ATOM_FP__FLAGS_COUNT_NAME"];\n"
             "extern const StaticOptionInfo ATOM_CT__OPTIONINFO["ATOM_FP__OPTION_START ATOM_FP__OPTIONS_COUNT_NAME"];\n"
             "\n",
             header
    );
}

void write_header_out_examble(FILE* header) {
    fputs("#endif //"ATOM_FP__HEADER_GUARD, header);
}

void write_header_out_option_arg_enums(FILE* h_file) {
    for (uint i = 0; i < optionInfos.pos; ++i) {
        const OptionInfo* info = optionInfos.arr[i];

        const size_t arg_c = info->arg_infos.pos;

        if (arg_c == 0) continue;

        const char* translated_name = translate_name_for_enum(info->option_name);
        fprintf(h_file, "typedef enum {\n");
        for (uint j = 0; j < arg_c; ++j) {
            const OptionArgInfo* args = info->arg_infos.arr[j];
            const char* translated_arg_name = translate_name_for_enum(args->arg_name);

            fprintf(h_file, "\tATOM_CT__OPTION_%s_ARG_%s,\n", translated_name, translated_arg_name);

            free((void*)translated_arg_name);
        }
        fprintf(h_file, "} ATOM_CT__OPTION_%s_ARGS_ENUM;\n\n", translated_name);

        for (int j = 0; j < arg_c; ++j) {
            const OptionArgInfo* args = info->arg_infos.arr[j];
            if (args->arg_options.pos == 0) continue;

            const char* translated_arg_name = translate_name_for_enum(args->arg_name);
            fprintf(h_file, "typedef enum {\n");
            for (int k = 0; k < args->arg_options.pos; ++k) {
                const OptionData* valid = arr_ptr(&args->arg_options, k);

                fprintf(h_file, "\tATOM_CT__OPTION_%s_ARG_%s_VALID_", translated_name, translated_arg_name);

                switch (args->type) {
                    case FP_TYPE_STR: {
                        const char* translated_valid_name = translate_name_for_enum(valid->str);
                        fprintf(h_file, "%s", translated_valid_name);
                        free((void*)translated_valid_name);
                        break;
                    }
                    case FP_TYPE_INTEGER:
                        fprintf(h_file, "%lld", valid->integer);
                        break;
                    case FP_TYPE_NATURAL:
                        fprintf(h_file, "%llu", valid->natural);
                        break;
                    case FP_TYPE_CHARACTER:
                        fprintf(h_file, "%c", valid->character);
                        break;
                }
                fprintf(h_file, ",\n");
            }
            fprintf(h_file, "} ATOM_CT__OPTION_%s_ARG_%s_VALID_ENUM;\n\n", translated_name, translated_arg_name);
            free((void*)translated_arg_name);
        }
        free((void*)translated_name);
    }
}

void write_header_out(FILE* header) {
    write_header_out_preamble(header);

    append_flag_enum(header);
    append_option_enum(header);

    write_header_out_amble(header);
    write_header_out_option_arg_enums(header);
    write_header_out_examble(header);
}

void write_c_out_preamble(FILE* c_file, const char* header_output_filename) {
    const char* file_name = get_file_name(header_output_filename);
    fprintf(c_file, "#include \"%s.h\"\n\n", file_name);
    free((void*)file_name);
}

void write_c_out_flag_info_data(FILE* c_file) {
    fprintf(c_file, "const "ATOM_FP__FLAG_INFO_STRUCT_NAME" "ATOM_FP__FLAG_INFO_INSTANCE_NAME"[%s] = {\n", ATOM_FP__FLAG_START ATOM_FP__FLAGS_COUNT_NAME);
    for (uint i = 0; i < flagInfos.pos; ++i) {
        const FlagInfo* info = flagInfos.arr[i];
        const char* translated_name = translate_name_for_enum(info->flag_name);

        fprintf(c_file, "    ["ATOM_FP__FLAG_START"%s] = ("ATOM_FP__FLAG_INFO_STRUCT_NAME"){."ATOM_FP__FLAG_INFO_STRING_NAME"= \"%s\", ."ATOM_FP__FLAG_INFO_DEFAULT_VAL_NAME"= %s},\n",
            translated_name,
            info->flag_name,
            info->default_value ? "true" : "false"
        );

        free((void*)translated_name);
    }
    fputs("};\n\n", c_file);
}

void write_c_out_flag_array_data(FILE* c_file) {
    fprintf(c_file, "bool "ATOM_FP__FLAGS_ARRAY_NAME"["ATOM_FP__FLAG_START ATOM_FP__FLAGS_COUNT_NAME"] = {\n");
    for (uint i = 0; i < flagInfos.pos; ++i) {
        const FlagInfo* info = flagInfos.arr[i];
        const char* value = info->default_value ? "true" : "false";

        fprintf(c_file, "    %s,\n", value);
    }
    fputs("};\n\n", c_file);
}

void write_c_out_option_info_data(FILE* c_file) {
    fprintf(c_file, "const Static"ATOM_FP__OPTION_INFO_STRUCT_NAME" "ATOM_FP__OPTION_INFO_INSTANCE_NAME"[%s] = {\n", ATOM_FP__OPTION_START ATOM_FP__OPTIONS_COUNT_NAME);
    for (uint i = 0; i < optionInfos.pos; ++i) {
        const OptionInfo* info = optionInfos.arr[i];
        const char* translated_name = translate_name_for_enum(info->option_name);

        fprintf(c_file, "\t["ATOM_FP__OPTION_START"%s]= (StaticOptionInfo) {\n", translated_name);
        fprintf(c_file, "\t\t.option_name= \"%s\",\n", info->option_name);
        fprintf(c_file, "\t\t.arg_info_count= %zu,\n", info->arg_infos.pos);
        fprintf(c_file, "\t\t.args= (StaticOptionArgInfo[]) {\n");

        for (uint j = 0; j < info->arg_infos.pos; ++j) {
            const OptionArgInfo* arg = info->arg_infos.arr[j];

            fprintf(c_file, "\t\t\t{\n");
            fprintf(c_file, "\t\t\t\t.arg_name= \"%s\",\n", arg->arg_name);
            fprintf(c_file, "\t\t\t\t.repeated= %s,\n", arg->repeated ? "true" : "false");
            fprintf(c_file, "\t\t\t\t.type= FP_TYPE_%s,\n", FP_TYPES_ENUM_STR[arg->type]);
            fprintf(c_file, "\t\t\t\t.arg_option_count= %u,\n", arg->arg_options.pos);
            fprintf(c_file, "\t\t\t\t.data= (OptionData[]) {\n");

            for (uint k = 0; k < arg->arg_options.pos; ++k) {
                const OptionData* data = arr_ptr(&arg->arg_options, k);

                fprintf(c_file, "\t\t\t\t\t{");
                switch (arg->type) {
                    case FP_TYPE_STR:
                        fprintf(c_file, ".str= \"%s\"", data->str);
                        break;
                    case FP_TYPE_INTEGER:
                        fprintf(c_file, ".integer= %lld", data->integer);
                        break;
                    case FP_TYPE_NATURAL:
                        fprintf(c_file, ".natural= %llu", data->natural);
                        break;
                    case FP_TYPE_CHARACTER:
                        fprintf(c_file, ".character= \'%c\'", data->character);
                        break;
                    default:
                        assert(false);
                }
                fprintf(c_file, "},\n");
            }
            fprintf(c_file, "\t\t\t\t},\n");
            fprintf(c_file, "\t\t\t},\n");
        }
        fprintf(c_file, "\t\t},\n");
        fprintf(c_file, "\t},\n");

        free((void*)translated_name);
    }
    fprintf(c_file, "};\n\n");
}

void write_c_out(FILE* c_file, const char* header_output_filename) {
    write_c_out_preamble(c_file, header_output_filename);
    write_c_out_flag_info_data(c_file);
    write_c_out_flag_array_data(c_file);
    write_c_out_option_info_data(c_file);
}

uint write_out_flag_data(const char* output_filename) {
    const char* temp_output_c_file_name = ATOM_FP__TEMP_FILENAME_STARTER".c";
    const char* temp_output_h_file_name = ATOM_FP__TEMP_FILENAME_STARTER".h";

    FILE* temp_c = fopen(temp_output_c_file_name, "w");
    FILE* temp_header = fopen(temp_output_h_file_name, "w");

    if (!temp_c || !temp_header) {
        error("Unable to open temporary file(s) for writing output. C: %s, H: %s\n", temp_c ? "OPENED" : "ERROR", temp_header ? "OPENED" : "ERROR");
        return EXIT_FAILURE;
    }

    write_header_out(temp_header);
    write_c_out(temp_c, output_filename);

    fclose(temp_c);
    fclose(temp_header);

    const char* c_out_filename = str_cat_dyn(output_filename, ".c");
    const char* h_out_filename = str_cat_dyn(output_filename, ".h");

    inform("Temp files written. (Over)Writing output files: `%s`, `%s`\n", c_out_filename, h_out_filename);

    // if the file exists then check it has been removed, else 0 (success)
    const int c_remove_res = access(c_out_filename, F_OK) == 0 ? remove(c_out_filename) : 0;
    const int c_remove_errno = errno;
    const int h_remove_res = access(h_out_filename, F_OK) == 0 ? remove(h_out_filename) : 0;
    const int h_remove_errno = errno;

    if (c_remove_res != 0 || h_remove_res != 0) {
        error("Unable to remove old files: `%s`: %s, `%s`: %s\n",
            c_out_filename,
            c_remove_res == 0 ? "REMOVED" : strerror(c_remove_errno),
            h_out_filename,
            h_remove_res == 0 ? "REMOVED" : strerror(h_remove_errno)
        );

        goto write_out_flag_data_out_file_cleanup;
    }

    const int c_rename_res = rename(temp_output_c_file_name, c_out_filename);
    const int c_rename_errno = errno;
    const int h_rename_res = rename(temp_output_h_file_name, h_out_filename);
    const int h_rename_errno = errno;

    if (c_rename_res != 0 || h_rename_res != 0) {
        error("Unable to rename file(s) for (Over)Writing: `%s`: %s, `%s`: %s\n",
            c_out_filename,
            c_rename_res == 0 ? "RENAMED" : strerror(c_rename_errno),
            h_out_filename,
            h_rename_res == 0 ? "RENAMED" : strerror(h_rename_errno)
        );

        goto write_out_flag_data_out_file_cleanup;
    }

    inform("All files written to. Exiting\n");

write_out_flag_data_out_file_cleanup:
    free((void*)c_out_filename);
    free((void*)h_out_filename);

    return EXIT_SUCCESS;
}

FlagInfo* create_flag_info(FPToken* identifier, FPToken* default_value) {
    if (!identifier || !default_value) {
        assert(false);
    }

    FlagInfo* info = malloc(sizeof(FlagInfo));

    *info = (FlagInfo) {
        .flag_name = identifier->str,
        .default_value = default_value->boolean
    };

    return info;
}

OptionArgInfo* create_arg_info(FPToken const* identifier, bool repeated, FPToken const* type, const Array arg_options) {
    OptionArgInfo* info = malloc(sizeof(OptionArgInfo));

    *info = (OptionArgInfo) {
        .arg_name = identifier->str,
        .repeated = repeated,
        .type = type->type_enum,
        .arg_options = arg_options
    };

    return info;
}

OptionInfo* create_option_info(const FPToken* identifier) {
    OptionInfo* info = malloc(sizeof(OptionInfo));

    *info = (OptionInfo){
        .arg_infos = vector_create(3),
        .option_name = identifier->str
    };

    return info;
}

char* get_word_end(char* word_start) {
    size_t i = 0;
    while (is_alph(word_start[i])) {i++;};
    return word_start + i;
}

char* get_flag_or_option_end(char* word_start) {
    size_t i = 0;
    while (is_alph_numeric(word_start[i]) || word_start[i] == '-') {i++;};
    return word_start + i;
}

int compare_strings_for_search(const void* stra, const void* strb) {
    return strcmp(stra, *(char**)strb);
}

const char* get_tptoken_type_str(const FPToken* token) {
    return token_types_str[token->type];
}

void print_token_data(const FPToken* token) {
    switch (token->type) {
        case FP_IDENTIFIER:
            printf("%s", token->str);
            break;
        case FP_TYPE:
            printf("%s", ATOM_FP__TYPES_STR[token->type_enum]);
            break;
        case FP_KEYWORD:
            printf("%s", FP_KEYWORD_STR[token->keyword_enum]);
            break;
        case FP_LIT_BOOL:
            printf("%s", token->boolean ? "true" : "false");
            break;
        case FP_LIT_INT:
            printf("%lld", token->integer);
            break;
        case FP_COMMA:
            putchar(',');
            break;
        case FP_COUNT:
            putz("How did we get here?");
            break;
        case FP_INVALID:
        default:
            putz("Invalid Data");
            break;
    }
}

void print_tptoken(const FPToken* token) {
    printf("TOK: %s (", get_tptoken_type_str(token));
    print_token_data(token);
    putchar(')');
}

FPToken lex_token(char* start) {
    FPToken t = {.type = FP_INVALID};

    if (is_whitespace(*start) || is_newline(*start) || *start == '\0') {
        l_pos++;

        return t;
    }

    if (*start == ',') {
        t.type = FP_COMMA;

        l_pos++;
        return t;
    }

    if (*start == '*') {
        t.type = FP_STAR;

        l_pos++;
        return t;
    }

    if (is_alph(*start)) {
        char* end = get_flag_or_option_end(start);
        const size_t bytes = end - start;

        if (!end) {
            assert(false);
        }

        if (bytes == STATIC_STRING_LEN("false") && strncmp(start, "false", bytes) == 0) {
            t.boolean = false;
            l_pos += STATIC_STRING_LEN("false");
        } else if (bytes == STATIC_STRING_LEN("true") && strncmp(start, "true", bytes) == 0) {
            t.boolean = true;
            l_pos += STATIC_STRING_LEN("true");
        } else {
            goto alph_cont;
        }

        t.type = FP_LIT_BOOL;
        return t;

    alph_cont:;
        const char end_save = *end;
        *end = '\0';
        const char* const* type_res = bsearch(start, ATOM_FP__TYPES_STR, FP_TYPES_COUNT, sizeof (ATOM_FP__TYPES_STR[0]), compare_strings_for_search);
        const char* const* keyword_res = bsearch(start, FP_KEYWORD_STR, FP_KEYWORD_COUNT, sizeof (FP_KEYWORD_STR[0]), compare_strings_for_search);
        *end = end_save;

        if (keyword_res && type_res) {
            assert(false);
        }

        if (keyword_res) {
            t.type = FP_KEYWORD;
            t.keyword_enum = keyword_res - FP_KEYWORD_STR;

            l_pos += strlen(*keyword_res);
        } else if (type_res) {
            t.type = FP_TYPE;
            t.type_enum = type_res - ATOM_FP__TYPES_STR;

            l_pos += strlen(*type_res);
        } else {
            // if it is not a keyword then it is an identifier
            char* ident = malloc(bytes + 1);

            if (!ident) {
                panic("Malloc failed when allocating identifier");
            }

            memcpy(ident, start, bytes);
            *(ident + bytes) = '\0';

            t.type = FP_IDENTIFIER;
            t.str = ident;

            l_pos += bytes;
        }
    } else if (is_digit(*start)) {
        char* end;
        const long long int value = strtoll(start, &end, 10);

        l_pos += end - start;

        t.type = FP_LIT_INT;
        t.integer = value;
    } else {
        warning("Found unidentifiable character `%c` (%d)\n", *start, *start);
        l_pos++;
    }

    return t;
}

uint lex_flag_file(FILE* file) {
    Buffer line_buffer = buffer_create(BUFF_MIN);
    tokens = arr_create(sizeof(FPToken));
    uint errcode = EXIT_SUCCESS;

    while (get_line(file, &line_buffer)) {
        if (starts_with_ips(line_buffer.data, ATOM_FP__COMMENT_START) != -1) {
            // ignore whitespace, no tokens for it
            continue;
        }

        if (line_buffer.pos < 1) {
            continue;
        }

        if (line_buffer.pos == 1) {
            error("Found a single character when lexing file; `%c`, no single character lines mean anything.\n", line_buffer.data[0]);
            errcode = EXIT_FAILURE;
            continue;
        }

        const char c = line_buffer.data[0];
        const char n = line_buffer.data[1];
        if ((c == '\n' || (c == '\r' && n == '\n')) && line_buffer.pos <= 2) {
            // skip empty lines
            continue;
        }

        l_pos = 0;
        while (l_pos < line_buffer.pos) {
            FPToken t = lex_token(&line_buffer.data[l_pos]);

            if (t.type == FP_INVALID) continue;

            arr_add(&tokens, &t);
        }
    }

    buffer_destroy(&line_buffer);

    for (uint i = 0; i < tokens.pos; ++i) {
        const FPToken* tok = arr_ptr(&tokens, i);
        print_tptoken(tok);
        newline();
    }

    return errcode;
}

FPToken* consume() {
    return arr_ptr(&tokens, p_pos++);
}

FPToken* peek() {
    return arr_ptr(&tokens, p_pos + 1);
}

FPToken* current() {
    return arr_ptr(&tokens, p_pos);
}

FPToken* expect(const TokenType type) {
    FPToken* tok = current();

    if (!tok) return NULL;

    if (tok->type != type) {
        return NULL;
    }

    return consume(); // eat the keyword (๑ᵔ⤙ᵔ๑)
}

FPToken* expect_keyword(const Keywords keyword) {
    const FPToken* tok = arr_ptr(&tokens, p_pos);

    if (tok->type != FP_KEYWORD) {
        return NULL;
    }

    if (tok->keyword_enum != keyword) {
        return NULL;
    }

    return consume();
}

uint parse_keyword_flag() {
    FPToken* identifier = expect(FP_IDENTIFIER);

    if (!identifier) {
        error("Expected an identifier after FLAG keyword\n");
        return EXIT_FAILURE;
    }

    FPToken* default_value = expect(FP_LIT_BOOL);

    if (!default_value) {
        error("Expected a lit boolean after Identifier in FLAG keyword\n");
        return EXIT_FAILURE;
    }

    FlagInfo* finfo = create_flag_info(identifier, default_value);

    if (!finfo) {
        assert(false);
    }

    vector_add(&flagInfos, finfo);

    return EXIT_SUCCESS;
}

uint parse_keyword_option() {
    FPToken* identifier = expect(FP_IDENTIFIER);

    if (!identifier) {
        error("Expected an identifier after OPTION keyword\n");
        return EXIT_FAILURE;
    }

    OptionInfo* info = create_option_info(identifier);
    vector_add(&optionInfos, info);

    return EXIT_SUCCESS;
}


uint parse_keyword_arg() {
    if (optionInfos.pos == 0) {
        error("Attempting to add argument, but no option defined before\n");
        return EXIT_FAILURE;
    }

    OptionInfo* last_option = vector_get_unsafe(&optionInfos, optionInfos.pos - 1);

    bool repeated = false;
    const FPToken* star_q = expect(FP_STAR);
    if (star_q) {
        repeated = true;
    }

    FPToken* identifier = expect(FP_IDENTIFIER);
    if (!identifier) {
        error("Expected identifier after keyword ARG for argument name\n");
        return EXIT_FAILURE;
    }

    FPToken* type_keyword = expect_keyword(FP_KEYWORD_TYPE);
    if (!type_keyword) {
        error("Expected type keyword after identifier in ARG statement\n");
        return EXIT_FAILURE;
    }

    FPToken* type = expect(FP_TYPE);
    if (!type) {
        error("Expected type after type keyword in ARG statement\n");
        return EXIT_FAILURE;
    }

    Array arg_options;

    switch (type->type_enum) {
        case FP_TYPE_STR:
            arg_options = arr_create(sizeof (char*));
            break;
        case FP_TYPE_INTEGER:
        case FP_TYPE_NATURAL:
            arg_options = arr_create(sizeof (long long int));
            break;
        case FP_TYPE_CHARACTER:
            arg_options = arr_create(sizeof (char));
            break;
        default:
            assert(false);
    }

    if (expect_keyword(FP_KEYWORD_FROM)) {
        bool first_time = true;

        while (first_time || expect(FP_COMMA)) {
            first_time = false;
            FPToken* value;

            switch (type->type_enum) {
                case FP_TYPE_STR:
                    value = expect(FP_IDENTIFIER);
                    if (!value) goto identifier_error;
                    arr_add(&arg_options, &value->str);
                    break;
                case FP_TYPE_INTEGER:
                case FP_TYPE_NATURAL:
                    value = expect(FP_LIT_INT);
                    if (!value) goto identifier_error;
                    arr_add(&arg_options, &value->integer);
                    break;
                case FP_TYPE_CHARACTER:
                    value = expect(FP_IDENTIFIER);
                    if (!value) goto identifier_error;
                    arr_add(&arg_options, value->str);
                    break;
                default:
                    assert(false);

            }
            continue;

        identifier_error:
            error("Expected identifier in FROM list\n");
            return EXIT_FAILURE;
        }

        const FPToken* n = current();
        if (n) {
            switch (n->type) {
                case FP_LIT_INT:
                case FP_LIT_BOOL:
                case FP_IDENTIFIER:
                    warning("Found non keyword after arguments list, you may be missing a comma in args list. Found: `%s` in arg list for `%s`\n",
                        token_types_str[n->type],
                        last_option->option_name
                    );
                default:;
            }
        }
    }

    OptionArgInfo* info = create_arg_info(identifier, repeated, type, arg_options);
    vector_add(&last_option->arg_infos, info);

    return EXIT_SUCCESS;
}

uint parse_token(const FPToken* token) {
    const Keywords enum_pos = token->keyword_enum;

    switch (enum_pos) {
        case FP_KEYWORD_FLAG:
            return parse_keyword_flag();
        case FP_KEYWORD_OPTION:
            return parse_keyword_option();
        case FP_KEYWORD_ARG:
            return parse_keyword_arg();
        default:
            error("Invalid keyword used as a statement starter. Found %s\n", FP_KEYWORD_STR[enum_pos]);
            break;
    }

    return EXIT_FAILURE;
}

uint parse_flag_file(FILE* file) {
    const uint ret = lex_flag_file(file);

    if (ret != EXIT_SUCCESS) {
        error("Errors generated from lexing, errcode %u\n", ret);
        return ret;
    }

    if (tokens.pos == 0) {
        warning("No tokens were generated from the lexing stage\n");
        return EXIT_SUCCESS;
    }

    while (peek()) {
        const FPToken* c = consume();

        if (c->type != FP_KEYWORD) {
            error("Found non keyword at start of statement parsing. Found: %s\n", token_types_str[c->type]);
            return EXIT_FAILURE;
        }

        parse_token(c);
    }

    return ret;
}

void print_flaginfo(const FlagInfo* info) {
    printf("`%s`: %s"C_RST,
        info->flag_name,
        info->default_value ? C_GRN"true" : C_RED"false"
    );
}

void print_all_flaginfos(const Vector* vec) {
    puts("FLAG INFOS:");
    for (uint i = 0; i < vec->pos; ++i) {
        const FlagInfo* info = vec->arr[i];

        print_flaginfo(info);
        if (i != vec->pos) newline();
    }
}

void print_arg_info(const OptionArgInfo* info) {
    printf("%s`%s`: `%s`", info->repeated ? C_BLU"REPEATED "C_RST : "", info->arg_name, ATOM_FP__TYPES_STR[info->type]);

    if (info->arg_options.pos == 0) {
        return;
    }

    putz(" From: ");
    for (uint i = 0; i < info->arg_options.pos; ++i) {
        switch (info->type) {
            case FP_TYPE_STR:
                printf("%s", *(char**)arr_ptr(&info->arg_options, i));
                break;
            case FP_TYPE_INTEGER:
                printf("%lld", *(long long*)arr_ptr(&info->arg_options, i));
                break;
            case FP_TYPE_NATURAL:
                printf("%llu", *(unsigned long long*)arr_ptr(&info->arg_options, i));
                break;
            case FP_TYPE_CHARACTER:
                printf("%c", *(char*)arr_ptr(&info->arg_options, i));
                break;
            default:
                assert(false);
        }
        if (i != info->arg_options.pos - 1) {
            putz(", ");
        }
    }
}

void print_option_info(const OptionInfo* info) {
    printf("`%s` Args: ", info->option_name);

    if (info->arg_infos.pos == 0) {
        putz("None");
        return;
    }

    for (uint i = 0; i < info->arg_infos.pos; ++i) {
        printf("\n    ");

        const OptionArgInfo* arg = vector_get_unsafe(&info->arg_infos, i);

        print_arg_info(arg);
    }
}

void print_all_options(const Vector* vec) {
    puts("OPTION INFOS:");
    for (uint i = 0; i < vec->pos; ++i) {
        const OptionInfo* const option = vec->arr[i];

        print_option_info(option);

        if (i != vec->pos) newline();
    }
}

bool verify_arguments(const int argc, char** argv) {
    if (argc != ARGS_COUNT) {
        usage("Format: ./Flag_Preprocessor <Flags file> <Output file>");
    }

    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            usage("NULL Argument, argument %d (1-indexed, ignoring ) is NULL", i);
        }
    }

    return EXIT_SUCCESS;
}
