#include "Commons.h"
#include "Parser/Node.h"
#include "shared_types.h"

#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

_Static_assert(KV_COUNT < 2 * 2 * 2, "builtin Idx currently only allows <= 8 keyvalues");

#include "enum-out.h"

extern OperandInfo OPERAND_INFO[41];
extern TypeMatrix COERCION_INFO;
extern const TypeLike TYPE_INFO[12];
extern CoercionRule COERCION_RULES[1];

#define TYPE_LIKE_COUNT GTYPE_COUNT

 static int get_from_type_matrix(TypeMatrix matrix, uint32_t x, uint32_t y) {
    uint32_t width= TYPE_LIKE_COUNT;
    uint64_t index= y * width + x;
    return (matrix[index / 8] >> (index & 7)) & 1;
}

#define TYPE_MATRIX_ROWS 16
// rc is ZERO INDEXED
static bool is_in_type_matrix(const TypeMatrix matrix, uint32_t rc, bool is_row) {
    if (is_row) {
        uint32_t s_k= (TYPE_LIKE_COUNT * rc);
        size_t s_b= floor((double)s_k / 8);
        size_t s_i= s_k % 8;
        uint8_t c= 0;

        for (size_t i= s_i; c < TYPE_LIKE_COUNT; i++) {
            if (i >= 8) {
                i= 0;
                s_b++;
            }
            if ((matrix[s_b] >> (7 - i)) & 1) {
                return true;
            }
            c++;
        }
        return false;
    }

    size_t index= rc;
    for (int r= 0; r < TYPE_MATRIX_ROWS; ++r) {
        while (index < 8) {
            if ((matrix[r] >> (7 - index)) & 1) return true;

            index += TYPE_LIKE_COUNT;
        }
        index %= 8;
    }
    return false;
}

struct TypeRes {
    bool succ;
    encodedType otype;
};

const struct TypeRes TYPE_RES_FAIL= {
    .succ= false,
    .otype= {}
};

struct TypeRes type_check_expr(
    Node* expr
) {

}

struct TypeRes type_check_op_bin(
    ATOM_CT__LEX_OPERATORS_ENUM op,
    encodedType left,
    encodedType right
) {
    const OperandInfo info= OPERAND_INFO[op];
    TypeMatrix matrix= info.matrix;

    if (left.is_builtin || right.is_builtin) {
        // builtin types must be removed via coercion
        if (left.is_builtin) {
            for (size_t i= 0; i < sizeof(COERCION_RULES); ++i) {
                const CoercionRule* rule= &COERCION_RULES[i];

                if (rule->left.is_builtin && rule->left.idx == left.builtin_idx) {
                    left.general= rule->right.idx;
                    left.is_builtin= false;
                    break;
                }
            }

            if (left.is_builtin) {
                return TYPE_RES_FAIL;
            }
        }
        if (right.is_builtin) {
            for (size_t i= 0; i < sizeof(COERCION_RULES); ++i) {
                const CoercionRule* rule= &COERCION_RULES[i];

                if (rule->left.is_builtin && rule->left.idx == right.builtin_idx) {
                    right.general= rule->right.idx;
                    right.is_builtin= false;
                    break;
                }
            }

            if (right.is_builtin) {
                return TYPE_RES_FAIL;
            }
        }
    }

    // check if the operand can accept the left and right type
    //  YES -> return type | explicit type
    //  NO -> check coercions
    //     VALID -> return type | explicit type
    //     NOT VALID -> return error

    const bool valid= get_from_type_matrix(matrix, left.general, right.general);

    if (valid) {
        switch (info.out_type) {
            case OUT_EXPLICIT: {
                TypeLike* tinfo= &TYPE_INFO[info.output_index];
                const TypeLikeInfo* base= (TypeLikeInfo*)&tinfo->t;

                ATOM_CT__LEX_TYPES_ENUM new_type= GENERAL_TO_TYPES[info.output_index];

                const size_t min_size= left.size > right.size ? left.size : right.size;
                size_t new_size= base->size;
                if (base->type == TL_TYPE && tinfo->t.has_variable_sizes) {
                    uint offset= 0;
                    for (size_t i= 0; i < tinfo->t.sizes.size; ++i) {
                        const unsigned int size= tinfo->t.sizes.arr[i];
                        if (size >= min_size) {
                            new_size= size;
                            offset= i;
                            break;
                        }
                    }
                    new_type += offset;
                }

                return (struct TypeRes){
                    .succ= true,
                    .otype= (encodedType){
                        .general= (ATOM_CT__LEX_TYPES_GENERAL_ENUM)info.output_index,
                        .type= new_type,
                        .size=new_size,
                        .tf_offset= 0
                    }
                };
            }
            case OUT_LEFT:
                return (struct TypeRes) {
                    .succ= true,
                    .otype= left
                };
            case OUT_RIGHT:
                return (struct TypeRes) {
                    .succ= true,
                    .otype= right
                };
            case OUT_UNWRAP:
                return (struct TypeRes) {
                    .succ= true,
                    .otype= (left.tf_offset--, left)
                };
            case OUT_WRAP:
                return (struct TypeRes) {
                    .succ= true,
                    .otype= (left.tf_offset++, left)
                };
            case OUT_BASED:
                return (struct TypeRes){
                    .succ= true,
                    .otype= left.size >= right.size ? left : right
                };
            default:
                assert(false);
        }
    }

    // INVALID; try coercions
    const bool l_v= is_in_type_matrix(matrix, left.general, false);
    const bool r_v= is_in_type_matrix(matrix, right.general, true);
    if (!l_v && !r_v) {
        printf("This is a no-link type error\n");
        return TYPE_RES_FAIL;
    }

    const bool target_l= left.size >= right.size;
    const bool target_r= right.size >= left.size;

    // if the target type (larger) isn't valid then we can't coerce
    if ((target_l && !l_v) || (!target_l && !r_v)) {
        printf("Invalid size link type error\n");
        return TYPE_RES_FAIL;
    }

    if (target_l) {
        // todo add some coercion statement for the AST

        // left one is the type we want to coerce the other into
        //  check if there is a coercion from right -> left
        if (get_from_type_matrix(COERCION_INFO, right.general, left.general)) {
            return (struct TypeRes) {
                .succ= true,
                .otype= left
            };
        }
    }

    if (target_r) {
        if (get_from_type_matrix(COERCION_INFO, left.general, right.general)) {
            return (struct TypeRes) {
                .succ= true,
                .otype= right
            };
        }
    }

    return TYPE_RES_FAIL;
}

typedef enum OP_TYPES {
    OT_
} OP_TYPES;


uint8_t arr[4]= {
    0b10011000,
    0b11010010,
    0b11010010,
    0b11010010
};

TypeMatrix test= (TypeMatrix) &arr;

static uint8_t OP_PLUS_MATRIX[]= {
    1,1,1
};