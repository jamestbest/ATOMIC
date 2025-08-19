typedef struct encodedType {
    uint64_t general: 4;    // 2^4= 16 different types
    uint64_t type: 5;       // 2^5= 32 different types
    uint64_t size: 4;       // 2^4= 16 bytes
    uint64_t tf_offset: 16; // typefix offset e.g. POINTER
} encodedType;

#include "enum-out.h"

struct {
    bool succ;
    encodedType otype;
} type_check_op_bin(
    ATOM_CT__LEX_OPERATORS_ENUM op,
    encodedType left,
    encodedType right
) {

}

typedef struct OpInfo {
    uint8_t precedence: 5;
    uint8_t assoc: 1;
    uint8_t type: 2;
} OpInfo;

typedef enum OP_TYPES {
    OT_
} OP_TYPES;


OpInfo OP_INFO[10]= {
    [OP_ARROW]= (OpInfo){.type=1,.precedence=1,.assoc=1},
};

uint8_t arr[4]= {
    0b10011000,
    0b11010010,
    0b11010010,
    0b11010010
};

TypeMatrix test= (TypeMatrix) &arr;