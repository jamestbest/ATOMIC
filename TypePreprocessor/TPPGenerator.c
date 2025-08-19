//
// Created by jamescoward on 10/06/2025.
//

#include "TPPGenerator.h"

#include "TPPGeneratorInternal.h"
#include "TPPParser.h"

static void generate_types(FILE* header_file) {
    fputs(
        "typedef struct OpInfo {\n"
        "    uint8_t precedence: 5;\n"
        "    uint8_t assoc: 1;\n"
        "    uint8_t type: 2;\n"
        "} OpInfo;\n\n",
        header_file
    );
}

static void generate_code_preamble(FILE* code_file, const char* header_file_name) {
    fprintf(
        code_file,
        "#include \"%s\"\n\n",
        header_file_name
    );
}

static void generate_header_preamble(FILE* header_file) {
    size_t type_count= 0;
    size_t gtype_count= types.pos + typefixes.pos;

    for (size_t i= 0; i < types.pos; ++i) {
        TypeInfo* info= TypeInfo_arr_ptr(&types, i);
        if (info->has_multiple_names) {
            type_count += info->names.pos;
        } else if (info->has_variable_sizes) {
            type_count += info->sizes.pos;
        } else {
            type_count += 1;
        }
    }

    fprintf(
        header_file,
        "#define TYPE_COUNT %zu\n"
        "#define GTYPE_COUNT %zu\n"
        "#define TYPEFIX_COUNT %zu\n\n",
        type_count,
        gtype_count,
        typefixes.pos
    );
}

static void generate_enums(FILE* enum_file) {
    // enums for
    /*
     *  - TYPES
     *      - GENERAL TYPES
     *      - ALL TYPES
     *  - OPERATORS
     */

    /*-----------------------------------/
     *                                  *
     *          GENERAL TYPES           *
     *                                  *
    /-----------------------------------*/
    fputs(
        "typedef enum ATOM_CT__LEX_TYPES_GENERAL_ENUM {\n",
        enum_file
    );

    for (size_t i= 0; i < typefixes.pos; ++i) {
        TypeFixInfo* typefix= TypeFixInfo_arr_ptr(&typefixes, i);

        fprintf(enum_file, "\t%s%s", GENERAL_ENUM_PREFIX, typefix->name);
        if (i != typefixes.pos - 1 || types.pos != 0) fputc(',', enum_file);
        putc('\n', enum_file);
    }

    for (size_t i= 0; i < types.pos; ++i) {
        TypeInfo* type= TypeInfo_arr_ptr(&types, i);

        fprintf(enum_file, "\t%s%s", GENERAL_ENUM_PREFIX, type->general_type);
        if (i != types.pos - 1) fputc(',', enum_file);
        putc('\n', enum_file);
    }

    fputs(
        "} ATOM_CT__LEX_TYPES_GENERAL_ENUM;\n\n",
        enum_file
    );

    /*-----------------------------------/
     *                                  *
     *          TYPES AFTER `:`         *
     *                                  *
    /-----------------------------------*/
    fputs(
        "typedef enum ATOM_CT__LEX_TYPES_ENUM {\n",
        enum_file
    );

    for (size_t i= 0; i < types.pos; ++i) {
        TypeInfo* type= TypeInfo_arr_ptr(&types, i);

        if (type->has_variable_sizes) {
            fputc('\t', enum_file);
            for (size_t j= 0; j < type->sizes.pos; ++j) {
                uint size= uint_arr_get(&type->sizes, j);
                fputz(enum_file, TYPE_ENUM_PREFIX);
                fputs_upper(enum_file, type->prefix);
                fprintf(
                    enum_file,
                    "%u",
                    size
                );
                if (j != type->sizes.pos - 1) fputz(enum_file, ", ");
            }
        } else if (type->has_multiple_names) {
            putc('\t', enum_file);
            for (size_t j= 0; j < type->names.pos; ++j) {
                const char* name= vector_get_unsafe(&type->names, j);
                fprintf(enum_file, "%s", TYPE_ENUM_PREFIX);
                fputs_upper(enum_file, name);

                if (j != type->names.pos - 1) fputz(enum_file, ", ");
            }
        } else {
            fprintf(enum_file, "\t%s", TYPE_ENUM_PREFIX);
            fputs_upper(enum_file, type->name);
        }

        if (i != types.pos - 1) fputc(',', enum_file);
        fprintf(enum_file, " // %s", type->general_type);
        putc('\n', enum_file);
    }

    fputs(
        "} ATOM_CT__LEX_TYPES_ENUM;\n\n",
        enum_file
    );

    /*-----------------------------------/
     *                                  *
     *             OPERATORS            *
     *                                  *
    /-----------------------------------*/
    fputs(
        "typedef enum ATOM_CT__LEX_OPERATORS_ENUM {\n",
        enum_file
    );

    for (size_t i= 0; i < operators.pos; ++i) {
        OperatorInfo* op= OperatorInfo_arr_ptr(&operators, i);
        fprintf(
            enum_file,
            "\t%s%-10s%c  // %s",
            OP_ENUM_PREFIX,
            op->name,
            (i != operators.pos - 1) ? ',' : ' ',
            op->symbol
        );
        putc('\n', enum_file);
    }

    fputs(
        "} ATOM_CT__LEX_OPERATORS_ENUM;\n\n",
        enum_file
    );
}

static void generate_arrays(FILE* code_file) {
    // arrays for
    /*
     *  - Strings
     *      - Types
     *      - Operators
     *  - Coercion info
     *  - Operands info
     */

    /*-----------------------------------/
     *                                  *
     *               TYPES              *
     *                                  *
    /-----------------------------------*/
    fprintf(
        code_file,
        "const char* %s[] = {\n",
        TYPES_ARRAY_NAME
    );
    for (size_t i= 0; i < types.pos; ++i) {
        TypeInfo* info= TypeInfo_arr_ptr(&types, i);

        fputz(code_file, "    ");
        if (info->has_variable_sizes) {
            for (size_t j= 0; j < info->sizes.pos; ++j) {
                const uint size= uint_arr_get(&info->sizes, j);
                fprintf(
                    code_file,
                    "\"%s%u\", ",
                    info->prefix,
                    size
                );
            }
        } else if (info->has_multiple_names) {
            for (size_t j= 0; j < info->names.pos; ++j) {
                const char* name= vector_get_unsafe(&info->names, j);
                fprintf(
                    code_file,
                    "\"%s\", ",
                    name
                );
            }
        } else {
            fprintf(
                code_file,
                "\"%s\", ",
                info->name
            );
        }
        putc('\n', code_file);
    }
    fputs(
        "};\n\n",
        code_file
    );

    /*-----------------------------------/
     *                                  *
     *           GENERAL TYPES          *
     *                                  *
    /-----------------------------------*/
    fprintf(
        code_file,
        "const char* %s[]= {\n",
        GENERAL_ARRAY_NAME
    );
    for (size_t i= 0; i < typefixes.pos; ++i) {
        TypeFixInfo* info= TypeFixInfo_arr_ptr(&typefixes, i);

        fprintf(
            code_file,
            "    \"%s\"",
            info->name
        );
        if (i != typefixes.pos || types.pos != 0) fputc(',', code_file);
        fnewline(code_file);
    }
    for (size_t i= 0; i < types.pos; ++i) {
        TypeInfo* info= TypeInfo_arr_ptr(&types, i);

        fprintf(
            code_file,
            "    \"%s\"",
            info->general_type
        );
        if (i != types.pos - 1) fputc(',', code_file);
        fnewline(code_file);
    }
    fputs(
        "};\n\n",
        code_file
    );

    /*-----------------------------------/
     *                                  *
     *            SMALL TYPES           *
     *                                  *
    /-----------------------------------*/
    fprintf(
        code_file,
        "const char* %s[]= {\n",
        TYPES_SMALL_ARRAY_NAME
    );
    for (size_t i= 0; i < typefixes.pos; ++i) {
        TypeFixInfo* info= TypeFixInfo_arr_ptr(&typefixes, i);

        fprintf(
            code_file,
            "    \"%s\"",
            info->alt_name
        );
        if (i != typefixes.pos - 1 || types.pos != 0) fputc(',', code_file);
        fnewline(code_file);
    }

    for (size_t i= 0; i < types.pos; ++i) {
        TypeInfo* info= TypeInfo_arr_ptr(&types, i);

        if (info->has_variable_sizes) {
            fprintf(
                code_file,
                "    \"%s\"",
                info->prefix
            );
        } else if (info->has_multiple_names) {
            fputz(code_file, "    ");
            for (size_t j= 0; j < info->names.pos; ++j) {
                const char* name= vector_get_unsafe(&info->names, j);

                fprintf(
                    code_file,
                    "\"%s\"",
                    name
                );
                if (j != info->names.pos - 1) fputc(',', code_file);
            }
        } else {
            fprintf(
                code_file,
                "    \"%s\"",
                info->name
            );
        }

        if (i != types.pos - 1) fputc(',', code_file);
        fnewline(code_file);
    }
    fputs(
        "};\n\n",
        code_file
    );

    /*-----------------------------------/
     *                                  *
     *             OPERATORS            *
     *                                  *
    /-----------------------------------*/
    fprintf(
        code_file,
        "const char* %s[]= {\n",
        OP_ARRAY_NAME
    );
    for (size_t i= 0; i < operators.pos; ++i) {
        OperatorInfo* info= OperatorInfo_arr_ptr(&operators, i);

        fprintf(
            code_file,
            "    \"%s\"",
            info->symbol
        );

        if (i != operators.pos - 1) fputc(',', code_file);
        fnewline(code_file);
    }
    fputs(
        "};\n\n",
        code_file
    );

    /*-----------------------------------/
     *                                  *
     *              OP INFO             *
     *                                  *
    /-----------------------------------*/
    fprintf(
        code_file,
        "OpInfo %s[%zu]= {\n",
        OP_INFO_ARRAY_NAME,
        operators.pos
    );

    for (size_t i= 0; i < operators.pos; ++i) {
        OperatorInfo* info= OperatorInfo_arr_ptr(&operators, i);

        int c= fprintf(
            code_file,
            "    [%s%s]=",
            OP_ENUM_PREFIX,
            info->name
        );

        if (i == 0) {
            fprintf(
                code_file,
                "%*s (OpInfo){.precedence=%u,.assoc=%u,.type=%u}",
                20 - c < 0 ? 0 : 20 - c,
                "",
                info->precedence,
                info->assoc,
                info->op_type
            );
        } else {
            fprintf(
                code_file,
                "%*s (OpInfo){%u, %u, %u}",
                20 - c < 0 ? 0 : 20 - c,
                "",
                info->precedence,
                info->assoc,
                info->op_type
            );
        }
        if (i != operators.pos - 1) fputc(',', code_file);
        fnewline(code_file);
    }
    fputs("};\n\n", code_file);

    /*-----------------------------------/
     *                                  *
     *          COERCION INFO           *
     *                                  *
    /-----------------------------------*/
    size_t tm_b= type_matrix_bytes();
    fprintf(
        code_file,
        "uint8_t COERCION_INTERNAL[%zu]= {\n",
        tm_b
    );

    for (uint i = 0; i < tm_b; ++i) {
        uint8_t row= coercions[i];

        fprintf(
            code_file,
            "0b%s%s",
            bit_rep[row >> 4],
            bit_rep[row & 0x0F]
        );

        if (i != tm_b - 1) putc(',', code_file);
        fnewline(code_file);
    }

    fprintf(
        code_file,
        "};\n\n"
    );

    fprintf(
        code_file,
        "TypeMatrix %s= (TypeMatrix) COERCION_INTERNAL;\n\n",
        COERCION_MATRIX_NAME
    );

}

errcode generate(FILE* header_output, FILE* code_output, const char* header_file_name) {
    generate_header_preamble(header_output);
    generate_types(header_output);
    generate_enums(header_output);

    fclose(header_output);

    generate_code_preamble(code_output, header_file_name);
    generate_arrays(code_output);

    fclose(code_output);

    return SUCC;
}
