//
// Created by jamescoward on 20/07/2024.
//

#include "TypePreprocessor.h"
#include "TypePreprocessorInternal.h"

#include "../SharedIncludes/Colours.h"
#include "../SharedIncludes/Helper_File.h"

#include <stdio.h>
#include <stdlib.h>

#include <time.h>

ARRAY_ADD(OperatorInfo, OperatorInfo)
ARRAY_ADD(Vector, Vector)

// FUNCTION SIGS
static void parse_enum_line(const char* line, Vector* enums);
static Vector collect_enums(FILE* file, const char* enum_name);

static void fprint_enum(FILE* file, const uint index, const char* name);
static void print_enums(const Vector* enums);
static void print_information(const Array* information);
static void print_operator_info(const OperatorInfo* info);


static Array parse_type_file_two(FILE* file, const Vector* type_enums, const Vector* operator_enums);
static Array parse_type_file(FILE* file, const Vector* type_enums);
static OperatorInfo parse_operator_type_line(const char* line, const Vector* types_enum);
void add_and_verify_type(const char* start, const uint length, Vector* types, const Vector* types_enum);

static uint get_matching_enum(const Vector* enums, const char* operator);
static void verify_operator_information(const Array* information);
static uint verify_enums_usage(const Vector* enums, const Array* information);

static void write_output_file(FILE* file, const char* file_name, const Array* information);

static void inform(const char* message, ...);
static void error(const char* message, ...);
static void panic(const char* message, ...);
static void warning(const char* message, ...);

static char await_choice(const char* reason);

static void fatal_file_error(const char* message);

uint errcode = EXIT_SUCCESS;
uint warncode = EXIT_SUCCESS;

#define NUMBER_OF_INPUTS 3

bool unsafe = false;

#define ENUM_FILE_OPERATOR_ENUM_NAME "ATOM_CT__LEX_OPERATORS_ENUM"
#define ENUM_FILE_TYPE_ENUM_NAME "ATOM_CT__LEX_TYPES_GENERAL_ENUM"

//./TP <Types File> <Operator enum file> <output file> (--UNSAFE)
int main(const int argc, char** argv) {
    if (argc < NUMBER_OF_INPUTS + 1 || argc > NUMBER_OF_INPUTS + 2) {
        printf(C_RED"USAGE: "C_RST"3 arguments are expected for type preprocessor in form ./TP <Types File> <Operator enum file> <output file> (--UNSAFE)\n");
        exit(EXIT_FAILURE);
    }

    /*  STEPS
     *  1. open types file & operator enum file
     *  2. parse to get [OPERATOR] [SYMMERTIC] [LIST OF ENUMS] [""]
     *  3. write to another file the const data - would know the length at preprocessor stage
     *
     *  If enum is missing or others are present then given an error
     */
    const char* type_file_name = argv[1];
    const char* enum_file_name = argv[2];
    const char* out_file_name = argv[3];

    if (argc == NUMBER_OF_INPUTS + 2) {
        const char* option = argv[4];

        if (str_eq(option, "--UNSAFE")) {
            unsafe = true;
            inform("Unsafe option given; errors will not cause immediate exit\n");
        }
        else warning("Invalid option given, only --UNSAFE is accepted\n");
    }

    inform("Collected file locations:\n\tType File: %s\n\tEnum File: %s\n\tOutput File: %s\n",
        type_file_name,
        enum_file_name,
        out_file_name
    );

    FILE* type_file = validate_file(type_file_name, "r");
    FILE* enum_file = validate_file(enum_file_name, "r");

    if (!type_file || !enum_file) {
        fatal_file_error("Unable to open read only type and enum file");
    }

    inform("File pointers:\n\tType File: %p\n\tEnum File: %p\n",
        type_file,
        enum_file
    );

    const Vector operator_enums = collect_enums(enum_file, ENUM_FILE_OPERATOR_ENUM_NAME);
    fseek(enum_file, 0, SEEK_SET);
    const Vector type_enums = collect_enums(enum_file, ENUM_FILE_TYPE_ENUM_NAME);

    print_enums(&operator_enums);
    print_enums(&type_enums);

    const Array tokens = parse_type_file_two(type_file, &type_enums, &operator_enums);

    for (uint i = 0; i < tokens.pos; ++i) {
        TPPToken* token = arr_get(&tokens, i);

        print_tpptoken(token);
        newline();
    }

    const Array operator_information = parse_type_file(type_file, &type_enums);

    verify_operator_information(&operator_information);
    verify_enums_usage(&operator_enums, &operator_information);

    print_information(&operator_information);

    if (errcode != EXIT_SUCCESS) {
        if (!unsafe) {
            error("Errors during parsing of files; please correct, file will not be written until errcode == EXIT_SUCCESS\n");
            return errcode;
        }

        const char choice = await_choice("Errors generated during parsing");

        if (choice == 'n') return EXIT_FAILURE;
    }
    else if (warncode != EXIT_SUCCESS) {
        const char choice = await_choice("Warnings generated during parsing");

        if (choice == 'n') return EXIT_FAILURE;
    }

    FILE* out_file = validate_file(out_file_name, "w");

    if (!out_file) {
        fatal_file_error("Unable to open a writable output file");
    }

    write_output_file(out_file, out_file_name, &operator_information);
}

void fatal_file_error(const char* message) {
    char* cwd = getcwd(NULL, 0);

    error("%s, unable to continue.\n\tcwd: %s", message, cwd);

    free(cwd);

    exit(EXIT_FAILURE);
}

char await_choice(const char* reason) {
    warning("%s. Please enter 'y' to continue the process, 'n' to end the process\n", reason);

    char a;
    uint count = 0;

    while (a = getchar(), a != 'y' && a != 'n') {
        if (a == '\n' || a == '\r') continue;

        warning("Invalid input please enter 'y' to continue the process, 'n' to end the process\n");

        if (++count > 2) {
            error("Mutliple invalid inputs; exiting process\n");
            a = 'n';
            break;
        }
    };

    return a;
}

void inform(const char* message, ...) {
    putz(C_BLU"INFO: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}

void warning(const char* message, ...) {
    warncode = EXIT_FAILURE;

    putz(C_MGN"WARNING: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}

void error(const char* message, ...) {
    errcode = EXIT_FAILURE;

    putz(C_RED"ERROR: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
}

void panic(const char* message, ...) {
    putz(C_RED"PANIC: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    fflush(stdout);

    exit(EXIT_FAILURE);
}

uint get_matching_enum(const Vector* enums, const char* operator) {
    for (uint i = 0; i < enums->pos; ++i) {
        const char* enumerator = vector_get_unsafe(enums, i);

        if (str_eq(enumerator, operator)) {
            return i;
        }
    }
    return -1;
}

uint verify_enums_usage(const Vector* enums, const Array* information) {
    bool enum_used[enums->pos];

    for (uint i = 0; i < enums->pos; ++i) {
        enum_used[i] = 0;
    }

    uint exitcode = EXIT_SUCCESS;

    for (uint i = 0; i < information->pos; ++i) {
        const OperatorInfo info = OperatorInfo_arr_get(information, i);

        const uint enum_matching = get_matching_enum(enums, info.operator);

        if (enum_matching == (uint)-1) {
            error("Found invalid operator enum `%s`, no known matching enum from enum file\n", info.operator);
            exitcode = EXIT_FAILURE;
        }
        else enum_used[enum_matching] = true;
    }

    for (uint i = 0; i < enums->pos; ++i) {
        if (!enum_used[i]) {
            warning("Operator enum `%s` not found in types file\n", vector_get_unsafe(enums, i));
        }
    }

    return exitcode;
}

const char* out_file_preamble = ""
        "//\n"
        "//  CREATED BY TypePreprocessor on %s\n"   // %s = time & date
        "//\n"
        "\n"
        "#ifndef %s_H\n"
        "#define %s_H\n"
        "\n"
        "#include \"../Lexer/Tokens.h\"\n"
        "\n"
        "typedef struct typeArray {\n"
        "    unsigned int size;\n"
        "    unsigned short types[];\n"
        "} typeArray;\n"
        "\n"
        "typedef struct ValidTypeOperations {\n"
        "    char symmetric;\n"
        "   unsigned int size;\n"
        "    const typeArray *general_types;\n"
        "} ValidTypeOperations;\n\n";

const char* out_file_examble = "#endif\n";

void write_operator_information(FILE* file, const OperatorInfo* info) {
    fprintf(file, "\t[%s] = (struct ValidTypeOperations){.symmetric = %s, .size = %u, .general_types = (typeArray[]){",
        info->operator,
        info->symmetrical ? "true" : "false",
        info->types.pos
    );

    for (uint i = 0; i < info->types.pos; ++i) {
        const Vector types = Vector_arr_get(&info->types, i);

        fprintf(file, "(typeArray){%zu, ", types.pos);

        for (uint j = 0; j < types.pos; ++j) {
            const char* type = vector_get_unsafe(&types, j);

            fprintf(file, "%s%s", type, j != types.pos - 1 ? ", " : "");
        }

        fprintf(file, "}%s", i != info->types.pos - 1 ? ", " : "");
    }

    fprintf(file, "}}");
}

void write_output_file(FILE* file, const char* file_name, const Array* information) {
    const time_t t = time(NULL);
    const struct tm* time_s = localtime(&t);

    char buff[64];
    const size_t ret = strftime(buff, sizeof (buff), "%c.c", time_s);

    const char* ending_name = get_file_name(file_name);

    fprintf(file, out_file_preamble,
        ret ? buff : "INVLD-DATE/TIME",
        ending_name,
        ending_name
    );

    fputs("const ValidTypeOperations valid_type_operations[] = {\n", file);

    for (uint i = 0; i < information->pos; ++i) {
        const OperatorInfo* info = arr_get(information, i);

        write_operator_information(file, info);

        fprintf(file,  i != information->pos - 1 ? ",\n" : "\n");
    }

    fputs("};\n\n", file);

    fputs(out_file_examble, file);
}

void verify_operator_information(const Array* information) {
    for (uint i = 0; i < information->pos; ++i) {
        const OperatorInfo info = OperatorInfo_arr_get(information, i);

        const Array types_arr = info.types;

        if (info.symmetrical && types_arr.pos > 1) {
            warning(
                "Logic issue: operator that is symmetrical but has multiple type sets. Found information on operator `%s` with %d sets\n",
                info.operator,
                types_arr.pos
            );
        }

        if (types_arr.pos == 0) {
            error(
                "Found operator with no type information. Found information on operator `%s`",
                info.operator
            );
        }

        if (types_arr.pos > 3) {
            error(
                "Operators with more than 3 operands are not known/supported by ATOMIC. Found information on operator `%s` to have %d operands\n",
                info.operator,
                info.types.pos
            );
        }

        for (uint j = 0; j < info.types.pos; ++j) {
            const Vector vec = Vector_arr_get(&info.types, j);

            if (vec.pos == 0) {
                error("Operator found with no types in listing. Found information on operator `%s` at index %d to have 0 types\n",
                    info.operator,
                    j
                );
            }
        }
    }
}

Array parse_type_file_two(FILE* file, const Vector* type_enums, const Vector* operator_enums) {
    Buffer line_buffer = buffer_create(100);

    tpplex_setup(type_enums, operator_enums);

    while (get_line(file, &line_buffer)) {
        tpplex_line(&line_buffer);
    }

    Array tokens = tpplex_end();

    // need to parse the tokens into the 3 types of statements
    /*
     *  1. ALIAS statements - these are aliases for the types
     *  2. Operator type statements - these give the types that can be used for the given type
     *  3. Type coercing statements - which types a type can be implicitly converted to
     */

    tpp_parse(tokens, )

    for (uint i = 0; i < tokens.pos; ++i) {
        TPPToken* token = arr_get(&tokens, i);

        if (token->type == ALIAS) {

        }
    }
}

Array parse_type_file(FILE* file, const Vector* type_enums) {
    Buffer buff = buffer_create(100);
    Array information = OperatorInfo_arr_create();

    while (get_line(file, &buff)) {
        const char start = buff.data[0];

        if (!is_alph(start)) {
            error("Type preprocessor does not support non-alphabetical starting lines in file. Ignoring line: `%s`\n",
                buff.data
            );
            continue;
        }

        OperatorInfo operator_types = parse_operator_type_line(buff.data, type_enums);
        arr_add(&information, &operator_types);
    }

    return information;
}

OperatorInfo parse_operator_type_line(const char* line, const Vector* types_enum) {
    OperatorSeekState state = SEEK_OPERATOR_NAME;

    uint idx = 0;
    uint start_pos = 0;
    char value;

    OperatorInfo info = (OperatorInfo){
        .operator = NULL,
        .symmetrical = false,
        .types = arr_construct(sizeof (Vector), 2)
    };

    while (value = line[idx], value != '\0') {
        switch (state) {
            case SEEK_OPERATOR_NAME: {
                if (value == '=') {
                    char* op_name = malloc(sizeof (char) * (idx + 1));

                    if (!op_name) exit(EXIT_FAILURE);

                    assert(start_pos == 0); // should always be start_pos == 0
                    memcpy(op_name, line, idx);
                    op_name[idx] = '\0';

                    info.operator = op_name;

                    state = SEEK_SYMMETRIC_CHECK;
                    start_pos = idx + 1;
                }
                else if (!is_alph(value)) {
                    warning("Malformed line in types file (ignoring); found symbol `%c.c` on line `%s` expected alphabetical or `=`\n",
                        value,
                        line
                    );
                    goto parse_operator_type_line_end;
                }

                break;
            }
            case SEEK_SYMMETRIC_CHECK: {
                if (value == ';') {
                    if (idx == start_pos) {
                        // there is no SYM keyword so just continue

                    sym_next_state:
                        state = SEEK_TYPES;
                        start_pos = idx;
                        continue;
                    }

                    const int cmp = memcmp(&line[start_pos], "SYM", 3);

                    if (cmp == 0) {
                        info.symmetrical = true;
                        goto sym_next_state;
                    }

                    warning("Malformed SYM character found (%.3s) on line `%s`\n",
                        &line[start_pos],
                        line
                    );
                    goto sym_next_state;
                }
                if (!is_alph(value)) {
                    warning("Malformed line in types file (ignoring); found symbol `%c.c` on line `%s` expected alphabetical or `;`\n",
                        value,
                        line
                    );
                }

                break;
            }
            case SEEK_TYPES: {
                if (value == ';') {
                    if (start_pos != idx) {
                        Vector* t = arr_get(&info.types, info.types.pos - 1);
                        add_and_verify_type(&line[start_pos], idx - start_pos, t, types_enum);
                    }

                    Vector types = vector_create(6);

                    arr_add(&info.types, &types);

                types_next_state:
                    start_pos = idx + 1;
                    idx++;
                    continue;
                }

                if (!is_alph_numeric(value) && value != '_') {
                    if (start_pos == idx) {
                        goto types_next_state;
                    }

                    Vector* t = arr_get(&info.types, info.types.pos - 1);
                    add_and_verify_type(&line[start_pos], idx - start_pos, t, types_enum);

                    goto types_next_state;
                }

                break;
            }
        }

        idx++;
    }

    if (start_pos != idx) {
        Vector* t = arr_get(&info.types, info.types.pos - 1);
        add_and_verify_type(&line[start_pos], idx - start_pos, t, types_enum);
    }

parse_operator_type_line_end:
    return info;
}

char* add_type(const char* start, const uint length, Vector* types) {
    char* type_store = malloc(sizeof (char) * (length + 1));

    if (!type_store) exit(EXIT_FAILURE);

    memcpy(type_store, start, length);
    type_store[length] = '\0';

    vector_add(types, type_store);

    return type_store;
}

void add_and_verify_type(const char* start, const uint length, Vector* types, const Vector* types_enum) {
    const char* enum_name = add_type(start, length, types);

    const uint match = get_matching_enum(types_enum, enum_name);

    if (match == (uint)-1) {
        error("Invalid Type given. Recieved type `%s`\n", enum_name);
    }
}

void print_operator_info(const OperatorInfo* info) {
    printf("Operator info printout:\n\tOperator: "C_YLW"%s"C_RST"\n\tSYM: %s"C_RST"\n", info->operator, info->symmetrical ? C_GRN"TRUE" : C_RED"FALSE");

    if (info->symmetrical)
        puts("\tTypes:");

    for (uint i = 0; i < info->types.pos; ++i) {
        Vector types = Vector_arr_get(&info->types, i);

        if (!info->symmetrical)
            printf("\tType index: "C_BLU"%d\n"C_RST, i);

        putz(C_MGN);
        for (uint j = 0; j < types.pos; j++) {
            const char* type = vector_get_unsafe(&types, j);
            printf("\t\t%s\n", type);
        }
        putz(C_RST);
    }
}

void print_information(const Array* information) {
    for (uint i = 0; i < information->pos; ++i) {
        OperatorInfo info = OperatorInfo_arr_get(information, i);
        print_operator_info(&info);
    }
}

Vector collect_enums(FILE* file, const char* enum_name) {
    Vector enums = vector_create(20);
    Buffer line_buff = buffer_create(100);

    char buff[50];
    const uint bytes_written = snprintf(buff, sizeof (buff), "typedef enum %s", enum_name);

    if (bytes_written >= sizeof (buff) - 1) {
        warning("Large enum name provided; may not be supported. Enum name %s, buff: %s",
            enum_name,
            buff
        );
    }

    bool in_enum_def = false;
    while (get_line(file, &line_buff)) {
        if (starts_with_ips(line_buff.data, buff) != -1) {
            if (in_enum_def) {
                panic("Logic error; entered enum definition while already inside enum definition");
            }
            in_enum_def = true;
            continue;
        }

        if (starts_with_ips(line_buff.data, "}") != -1 && in_enum_def) {
            in_enum_def = false;
            break;
        }

        if (!in_enum_def) continue;

        parse_enum_line(line_buff.data, &enums);
    }

    return enums;
}

void add_enum(const char* start, const uint length, Vector* enums) {
    char* enum_store = malloc(sizeof (char) * (length + 1));

    if (!enum_store) exit(EXIT_FAILURE);

    memcpy(enum_store, start, length);
    enum_store[length] = '\0';

    vector_add(enums, enum_store);
}

void parse_enum_line(const char* line, Vector* enums) {
    uint pos = 0;
    uint enum_start = -1;
    bool in_enum_def = false;

    while (line[pos] != '\0') {
        const char current = line[pos];

        if (current == '/') {
            warning("TypePreprocessor does not support C comments inside enum definition; line: %s\n",
                line
            );
            break;
        }

        if (current == '}') warning(C_MGN"WARNING: "C_RST"TypePreprocessor does not support enums on same line as ending brace");

        if (!is_alph_numeric(current) && current != '_') {
            if (in_enum_def) {
                add_enum(&line[enum_start], pos - enum_start, enums);
            }

            in_enum_def = false;
        } else {
            if (!in_enum_def) enum_start = pos;

            in_enum_def = true;
        }

        pos++;
    }

    if (in_enum_def) {
        add_enum(&line[enum_start], pos - enum_start, enums);
    }
}

void fprint_enum(FILE* file, const uint index, const char* name) {
    fprintf(file, "  VAL: %u; STR: %s\n", index, name);
}

void print_enums(const Vector* enums) {
    inform("Information on enums collected\n");

    if (!vector_verify(enums)) {
        error("Enum vector found with issues: ptr=%p; arr=%p; cap=%llu; pos=%llu",
            enums,
            enums->arr,
            enums->capacity,
            enums->pos
        );
        exit(EXIT_FAILURE);
    }

    for (uint i = 0; i < enums->pos; ++i) {
        const char* enum_name = enums->arr[i];

        fprint_enum(stdout, i, enum_name);
    }
}

void bit_set(uint64_t* field, uint bit) {
    *field |= (1 << bit);
}

uint bit_get(uint64_t field, uint bit) {
    return (field >> bit) & 0x1;
}

void set_type_matrix(TypeMatrix* matrix, uint x, uint y) {
    if (y > 7) bit_set(&matrix->bottom_pad, y * 8 + x);
    else bit_set(&matrix->top_pad, y * 8 + x);
}

uint get_type_matrix(TypeMatrix* matrix, uint x, uint y) {
    if (y > 7) return bit_get(matrix->bottom_pad, y * 8 + x);
    else return bit_get(matrix->top_pad, y * 8 + x);
}