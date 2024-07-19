//
// Created by jamescoward on 15/07/2024.
//

#ifndef STATICVERIFICATIONINTERNAL_H
#define STATICVERIFICATIONINTERNAL_H

#include "../Lexer/Tokens.h"

typedef struct ValidTypeOperations {
    bool symmetric;
    unsigned short *general_types;
    unsigned short *second_types;
    unsigned short *third_types;
} ValidTypeOperations;

/* INTEGER
 * NATURAL
 * REAL
 * RATIONAL
 * STRING
 * CHAR
 * BOOLEAN
 * NOT_A_VALUE
 */

// const ValidTypeOperations valid_type_operations[] = {
//     [PLUS] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short*){NUMERIC, ALPHABETICAL, POINTER}},
//     [MINUS] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short[][]){{NUMERIC, POINTER}}},
//     [MULT] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short[][]){{NUMERIC, POINTER}}},
//     [DIV] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short[][]){{NUMERIC, POINTER}}},
//     [MOD] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short[][]){{NUMERIC, POINTER}}},
//     [POW] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short[][]){{NUMERIC}}},
//
//     [BAND] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short[][]){{INTEGER, NATURAL, BOOL, CHAR}}},
//     [BOR] = (struct ValidTypeOperations){.symmetric = true, .general_types = (unsigned short[][]){{INTEGER, NATURAL, BOOL, CHAR}}},
//
//     [SHL] = {.symmetric = true, .general_types = (unsigned short[][]){{INTEGER, NATURAL}}},
//     [SHR] = {.symmetric = true, .general_types = (unsigned short[][]){{INTEGER, NATURAL}}},
//
//     [ASS_PLUS] = {},
//     [ASS_MINUS] = {},
//     [ASS_MULT] = {},
//     [ASS_DIV] = {},
//     [ASS_MOD] = {},
//     [ASS_POW] = {},
//
//     [ASS_BAND] = {},
//     [ASS_BOR] = {},
//
//     [ASS_SHL] = {},
//     [ASS_SHR] = {},
//
//     [LAND] = {},
//     [LOR] = {},
//     [LXOR] = {},
//     [BXOR] = {},
//
//     [LNOT] = {},
//     [BNOT] = {},
//     [INC] = {},
//     [DEC] = {},
//
//     [QUESTION] = {},
//
//     [AMPERSAND] = {},
//
//     [TYPE_CONVERSION] = {},
//
//     [ASSIGNMENT] = {},
//
//     [EQU] = {},
//     [NEQ] = {},
//     [LESS] = {},
//     [MORE] = {},
//     [LESSEQ] = {},
//     [MOREEQ] = {},
//
//     [DEREFERENCE] = {},
//
//     [SWAP] = {},
//     [RANGE] = {},
//     [ARROW] = {},
// };

#endif //STATICVERIFICATIONINTERNAL_H
