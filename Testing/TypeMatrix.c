//
// Created by jamescoward on 26/08/2025.
//

#include "TypeMatrix.h"

#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <TypePreprocessor/output/enum-out.h>

#define TYPE_COUNT 4
#define GTYPE_COUNT 11
#define TYPEFIX_COUNT 1
#define TYPE_LIKE_COUNT (TYPE_COUNT + TYPEFIX_COUNT)

typedef uint8_t* TypeMatrix;

typedef struct encodedType {
    uint64_t general: 4;    // 2^4= 16 different types
    uint64_t type: 5;       // 2^5= 32 different types
    uint64_t size: 4;       // 2^4= 16 bytes
    uint64_t tf_offset: 16; // typefix offset e.g. POINTER
    // todo to support multiple tfs need a map
} encodedType;

struct TypeRes {
    bool succ;
    encodedType otype;
};

struct TypeRes type_check_op_bin(
    ATOM_CT__LEX_OPERATORS_ENUM op,
    encodedType left,
    encodedType right
);

#define TYPE_MATRIX_ROWS 4
// rc is ZERO INDEXED
static inline bool is_in_type_matrix(const TypeMatrix matrix, uint32_t rc, bool is_row) {
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

extern const char* ATOM_CT__LEX_TYPES_GENERAL_RAW[];

int main() {
    struct TypeRes b= type_check_op_bin(
        OP_LOR,
        (encodedType) {
            .general= GTYPE_INTEGER,
            .type= TYPE_I8,
            .size= 8,
            .tf_offset= 0
        },
        (encodedType) {
            .general= GTYPE_POINTER,
            .type= TYPE_CHR,
            .size= 8,
            .tf_offset= 1
        }
    );

    printf(
        "Res:\n"
        "  succ: %d\n"
        "  type: %s(%d)\n\n",
        b.succ,
        ATOM_CT__LEX_TYPES_GENERAL_RAW[b.otype.general],
        b.otype.size
    );

    TypeMatrix a;

    uint8_t arr[4]= {
        0b01000001,
        0b01001000,
        0b00000001,
        0b10000000
    };

    a= arr;

//    for (int i= 0; i < 5; ++i) {
//        printf("is_in col %d: %d\n", i, is_in_type_matrix(a, i, false));
//    }
//
//    for (int i= 0; i < 5; ++i) {
//        printf("is_in row %d: %d\n", i, is_in_type_matrix(a, i, true));
//    }

    return 0;
}