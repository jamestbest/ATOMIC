//
// Created by jamescoward on 10/06/2025.
//

#include "TPPGenerator.h"

#include "SharedIncludes/Helper_File.h"
#include "TPPGeneratorInternal.h"
#include "TPPParser.h"

static void output_internal_matrix(FILE* file, const char* name, const char* name_postfix, TypeMatrix matrix, size_t tm_b);

static void generate_types(FILE* header_file) {
    fputs("#include \"TPPHeader.h\"\n\n", header_file);
}

static void generate_code_preamble(FILE* code_file, const char* header_file_name) {
    char* just_file_name= get_file_name(header_file_name);

    fprintf(
        code_file,
        "#include \"%s\"\n\n",
        just_file_name
    );

    free(just_file_name);

    fputs(
        "#include <stdint.h>\n\n",
        code_file
    );
}

static void generate_header_extern(FILE* header_file) {
    fprintf(
        header_file,
        "extern ATOM_CT__LEX_TYPES_ENUM GENERAL_TO_TYPES[GTYPE_COUNT];\n"
        "extern ATOM_CT__LEX_TYPES_GENERAL_ENUM TYPES_TO_GENERAL[TYPE_COUNT];\n"
        "\n"
        "extern const char* ATOM_CT__LEX_TYPES_RAW[TYPE_COUNT];\n"
        "extern const char* ATOM_CT__LEX_TYPES_GENERAL_RAW[GTYPE_COUNT];\n"
        "extern const char* ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW[GTYPE_COUNT];\n"
        "extern const char* ATOM_CT__LEX_OPERATORS_RAW[OP_COUNT];\n"
        "\n"
        "extern const TypeLike TYPE_INFO[GTYPE_COUNT];\n"
        "\n"
    );
}

static void generate_header_examble(FILE* header_file) {
    fprintf(
        header_file,
        "#endif // ENUMOUT_H\n"
    );
}

static void generate_header_preamble(FILE* header_file) {
    fprintf(
        header_file,
        "#ifndef ENUMOUT_H\n"
        "#define ENUMOUT_H\n\n"
    );

    size_t type_count= 0;
    const size_t gtype_count= typelikes.pos;

    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* base= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        if (base->type != TL_TYPE) continue;

        const TypeInfo* info= (TypeInfo*)base;
        if (info->has_variable_sizes) {
            type_count += info->sizes.pos;
        } else {
            type_count += 1;
        }
    }

    fprintf(
        header_file,
        "#define TYPE_COUNT %zu\n"
        "#define GTYPE_COUNT %zu\n"
        "#define TYPEFIX_COUNT %zu\n"
        "#define OP_COUNT %zu\n\n",
        type_count,
        gtype_count,
        typefix_mirror.pos,
        operators.pos
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

    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* tl= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        fprintf(
            enum_file,
            "\t%s%s%c\n",
            GENERAL_ENUM_PREFIX,
            tl->general_type,
            i != typelikes.pos - 1 ? ',' : ' '
        );
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

    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* base= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        if (base->type != TL_TYPE) continue;
        TypeInfo* type= (TypeInfo*)base;

        if (type->has_variable_sizes) {
            fputc('\t', enum_file);
            for (size_t j= 0; j < type->sizes.pos; ++j) {
                uint size= uint_arr_get(&type->sizes, j);
                fputz(enum_file, TYPE_ENUM_PREFIX);
                fputs_upper(enum_file, base->prefix);
                fprintf(
                    enum_file,
                    "%u",
                    size
                );
                if (j != type->sizes.pos - 1) fputz(enum_file, ", ");
            }
        } else {
            fprintf(enum_file, "\t%s", TYPE_ENUM_PREFIX);
            fputs_upper(enum_file, base->name);
        }

        fprintf(enum_file, ", // %s\n", base->general_type);
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

static void generate_RuleValue(FILE* file, const RuleValue* rule, const char* prefix, const char* entry_name, bool last_entry) {
    fprintf(
        file,
        "%s.%s= {\n"
        "%s  .is_builtin= %u,\n"
        "%s  .idx= %u\n"
        "%s}%c\n",
        prefix,
        entry_name,
        prefix,
        rule->is_builtin,
        prefix,
        rule->idx,
        prefix,
        last_entry ? ' ' : ','
    );
}

static void generate_arrays(FILE* code_file) {
    // arrays for
    /*
     *  - Strings
     *      - Types
     *      - Operators
     *  - Type     info
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
    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* base= TypeLikeInfo_vec_get_unsafe(&typelikes, i);
        if (base->type != TL_TYPE) continue;

        TypeInfo* info= (TypeInfo*)base;

        fputz(code_file, "    ");
        if (info->has_variable_sizes) {
            for (size_t j= 0; j < info->sizes.pos; ++j) {
                const uint size= uint_arr_get(&info->sizes, j);
                fprintf(
                    code_file,
                    "\"%s%u\", ",
                    base->prefix,
                    size
                );
            }
        } else {
            fprintf(
                code_file,
                "\"%s\", ",
                base->name
            );
        }
        fnewline(code_file);
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
    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* base= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        fprintf(
            code_file,
            "    \"%s\"%c\n",
            base->general_type,
            i != typelikes.pos - 1 ? ',' : ' '
        );
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
    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* base= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        fprintf(
            code_file,
            "    \"%s\"%c\n",
            base->name,
            i != typelikes.pos - 1 ? ',' : ' '
        );
    }
    fputs(
        "};\n\n",
        code_file
    );

    /*-----------------------------------/
     *                                  *
     *           GTYPE to TYPE          *
     *                                  *
    /-----------------------------------*/
    fprintf(
        code_file,
        "ATOM_CT__LEX_TYPES_ENUM GENERAL_TO_TYPES[GTYPE_COUNT]= {\n"
    );
    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* info= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        fprintf(
            code_file,
            "\t[GTYPE_%s]= ",
            info->general_type
        );

        if (info->type == TL_TYPE_FIX) {
            fputs("-1", code_file);
        } else if (info->type == TL_TYPE) {
            const TypeInfo* type= (TypeInfo*)info;
            fputs("TYPE_", code_file);
            fputs_upper(code_file, info->name);

            if (type->has_variable_sizes) {
                fprintf(
                    code_file,
                    "%d",
                    uint_arr_get(&type->sizes, 0)
                );
            }
        } else {
            assert(false);
        }

        fputc(i != typelikes.pos - 1 ? ',' : ' ', code_file);
        fnewline(code_file);
    }
    fprintf(
        code_file,
        "};\n\n"
    );

    /*-----------------------------------/
     *                                  *
     *           TYPE to GTYPE          *
     *                                  *
    /-----------------------------------*/
    fprintf(
        code_file,
        "ATOM_CT__LEX_TYPES_GENERAL_ENUM TYPES_TO_GENERAL[TYPE_COUNT]= {\n"
    );
    for (size_t i= 0; i < typelikes.pos; ++i) {
        const TypeLikeInfo* info= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        if (info->type != TL_TYPE) continue;

        const TypeInfo* type= (TypeInfo*)info;
        if (type->has_variable_sizes) {
            for (size_t j= 0; j < type->sizes.pos; ++j) {
                uint size= uint_arr_get(&type->sizes, j);

                fprintf(
                    code_file,
                    "\t[TYPE_"
                );
                fputs_upper(code_file, info->name);
                fprintf(
                    code_file,
                    "%u]= GTYPE_%s",
                    size,
                    info->general_type
                );
                fputc((i != typelikes.pos - 1) || (j != type->sizes.pos - 1) ? ',' : ' ', code_file);
                fnewline(code_file);
            }
        } else {
            fprintf(
                code_file,
                "\t[TYPE_"
            );
            fputs_upper(code_file, info->name);
            fprintf(
                code_file,
                "]= GTYPE_%s",
                info->general_type
            );

            fputc(i != typelikes.pos - 1 ? ',' : ' ', code_file);
            fnewline(code_file);
        }
    }
    fprintf(
        code_file,
        "};\n\n"
    );



    /*-----------------------------------/
     *                                  *
     *             TYPE INFO            *
     *                                  *
    /-----------------------------------*/
    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* base= TypeLikeInfo_vec_get_unsafe(&typelikes, i);
        if (base->type != TL_TYPE) continue;

        TypeInfo* info= (TypeInfo*)base;

        if (info->requirements.pos) {
            fprintf(
                code_file,
                "const unsigned int %s_REQS[%zu]= {\n",
                base->general_type,
                info->requirements.pos
            );
            for (size_t j= 0; j < info->requirements.pos; ++j) {
                uint req= uint_arr_get(&info->requirements, j);
                fprintf(
                    code_file,
                    "   %u%c\n",
                    req,
                    j != info->sizes.pos - 1 ? ',' : ' '
                );
            }
            fprintf(
                code_file,
                "};\n\n"
            );
        }

        if (info->has_variable_sizes) {
            fprintf(
                code_file,
                "const unsigned int %s_SIZES[%zu]= {\n",
                base->general_type,
                info->sizes.pos
            );
            for (size_t j= 0; j < info->sizes.pos; ++j) {
                uint size= uint_arr_get(&info->sizes, j);
                fprintf(
                    code_file,
                    "   %u%c\n",
                    size,
                    j != info->sizes.pos - 1 ? ',' : ' '
                );
            }
            fprintf(
                code_file,
                "};\n\n"
            );
        }
    }

    fprintf(
        code_file,
        "const TypeLike %s[%zu]= {\n",
        TYPE_INFO_ARRAY_NAME,
        typelikes.pos
    );
    for (size_t i= 0; i < typelikes.pos; ++i) {
        TypeLikeInfo* base= TypeLikeInfo_vec_get_unsafe(&typelikes, i);

        TypeInfo* t= (TypeInfo*)base;
        TypeFixInfo* tf= (TypeFixInfo*)base;

        fprintf(
            code_file,
            "   [GTYPE_%s]=(TypeLike){\n"
            "       .%s= {\n"
            "           .base= (TypeLikeInfo) {\n"
            "               .type=%s,\n"
            "               .size=%u\n"
            "           },\n",
            base->general_type,
            base->type == TL_TYPE ? "t" : base->type == TL_TYPE_FIX ? "tf" : "ERROR_BAD_TYPE",
            base->type == TL_TYPE ? "TL_TYPE" : base->type == TL_TYPE_FIX ? "TL_TYPE_FIX" : "ERROR_BAD_TYPE",
            base->size
        );
        if (base->type == TL_TYPE) {
            fprintf(
                code_file,
                "           .has_variable_sizes= %u,\n"
                "           .is_virtual= %u,\n",
                t->has_variable_sizes,
                t->is_virtual
            );

            if (t->has_variable_sizes) {
                fprintf(
                    code_file,
                    "           .sizes= (uintArrayS) {\n"
                    "               .size= %zu,\n"
                    "               .arr= %s_SIZES\n"
                    "           },\n",
                    t->sizes.pos,
                    base->general_type
                );
            } else {
                fprintf(
                    code_file,
                    "           .sizes= {.size= 0},\n"
                );
            }

            if (t->requirements.pos) {
                fprintf(
                    code_file,
                    "           .reqs= (uintArrayS) {\n"
                    "               .size= %zu,\n"
                    "               .arr= %s_REQS\n"
                    "           }\n",
                    t->requirements.pos,
                    base->general_type
                );
            } else {
                fprintf(
                    code_file,
                    "           .reqs= {.size= 0},\n"
                );
            }
        } else if (base->type == TL_TYPE_FIX) {
            fprintf(
                code_file,
                "           .prefix= %u\n",
                tf->prefix
            );
        }

        fprintf(
            code_file,
            "       }\n"
            "   }%c\n",
            i != typelikes.pos - 1 ? ',' : ' '
        );
    }
    fprintf(
        code_file,
        "};\n\n"
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
    output_internal_matrix(
        code_file,
        "COERCION",
        "_INTERNAL",
        coercions,
        tm_b
    );

    fprintf(
        code_file,
        "TypeMatrix %s= COERCION_INTERNAL;\n\n",
        COERCION_MATRIX_NAME
    );

    fprintf(
        code_file,
        "CoercionRule COERCION_RULES[%zu]= {\n",
        coercion_rules.pos
    );
    for (size_t i= 0; i < coercion_rules.pos; ++i) {
        const CoercionRule* rule= CoercionRule_arr_ptr(&coercion_rules, i);

        fprintf(
            code_file,
            "  [%zu]= (CoercionRule){\n",
            i
        );
        generate_RuleValue(code_file,
            &rule->left,
            "    ",
            "left",
            false
        );
        generate_RuleValue(code_file,
            &rule->right,
            "    ",
            "right",
            true
        );
        fprintf(
            code_file,
           "  }%c\n",
           i != coercion_rules.pos - 1 ? ',' : ' '
        );
    }
    fprintf(
        code_file,
        "};\n\n"
    );

    /*-----------------------------------/
     *                                  *
     *           OPERAND INFO           *
     *                                  *
    /-----------------------------------*/

    // we need the type matrix for all BINARY operands
    for (size_t i= 0; i < operands.pos; ++i) {
        const OperandInfo* info= OperandInfo_arr_ptr(&operands, i);
        const OperandInfoBase* base= (OperandInfoBase*)info;

        if (base->is_keyvalue) continue;
        if (base->op_type != OIGT_BINARY) continue;

        TypeMatrix matrix= base->matrix;

        output_internal_matrix(
            code_file,
            info->operator->name,
            "_MATRIX",
            matrix,
            tm_b
        );
    }

    fprintf(code_file,
            "OperandInfo OPERAND_INFO[%zu]= {\n",
            operators.pos
    );

    // now we have the main bulk of information
    for (size_t i= 0; i < operands.pos; ++i) {
        const OperandInfo* info= OperandInfo_arr_ptr(&operands, i);
        const OperandInfoBase* base= (OperandInfoBase*)info;

        // E.G. [OP_PLUS]= (OperandInfo){\n
        fprintf(code_file,
                "\t[%s%s]= (OperandInfo){\n",
                OP_ENUM_PREFIX,
                info->operator->name
        );

        if (base->is_keyvalue) {
            generate_RuleValue(
                code_file,
                &info->base.left,
                "\t\t",
                "left",
                false
            );

            if (base->op_type == OIGT_BINARY) {
                generate_RuleValue(
                    code_file,
                    &info->base.right,
                    "\t\t",
                    "right",
                    false
                );
            }
        } else {
            // E.G. \t\t.matrix= PLUS_MATRIX
            //      \t\t.typemap= 123
            if (base->op_type == OIGT_BINARY) {
                fprintf(code_file,
                    "\t\t.matrix= %s%s,\n",
                    info->operator->name,
                    "_MATRIX"
                );
            } else if (base->op_type == OIGT_PREFIX || base->op_type == OIGT_POSTFIX) {
                fprintf(code_file,
                    "\t\t.typemap=%lld,\n",
                    base->typemap
                );
            } else {
                assert(false);
            }
        }

        fprintf(code_file,
            "\t\t.op_type= %u,\n"
                "\t\t.out_type= %s,\n"
                "\t\t.output_index= %u,\n"
                "\t\t.is_keyvalue= %u\n"
                "\t}%c\n",
            base->op_type,
            OUTPUT_TYPE_STRINGS[base->out_type],
            base->output_index,
            base->is_keyvalue,
            i == operands.pos - 1 ? ' ' : ','
        );
    }
    fprintf(code_file,
        "};\n\n"
    );
}

static void output_internal_matrix(FILE* file, const char* name, const char* name_postfix, TypeMatrix matrix, size_t tm_b) {
    fprintf(
        file,
        "static uint8_t %s%s[%zu]= {\n",
        name,
        name_postfix,
        tm_b
    );

    for (uint i = 0; i < tm_b; ++i) {
        uint8_t row= matrix[i];

        fprintf(
            file,
            "\t0b%s%s",
            bit_rep[row >> 4],
            bit_rep[row & 0x0F]
        );

        if (i != tm_b - 1) putc(',', file);
        fnewline(file);
    }

    fprintf(
        file,
        "};\n\n"
    );
}

errcode generate(FILE* header_output, FILE* code_output, const char* header_file_name) {
    generate_header_preamble(header_output);
    generate_types(header_output);
    generate_enums(header_output);
    generate_header_extern(header_output);
    generate_header_examble(header_output);

    fclose(header_output);

    generate_code_preamble(code_output, header_file_name);
    generate_arrays(code_output);

    fclose(code_output);

    return SUCC;
}
