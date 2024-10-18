//
//  CREATED BY TypePreprocessor on 07/23/24 19:56:19.c
//

#ifndef TypeConstInfo_H
#define TypeConstInfo_H

#include "../Lexer/Tokens.h"

typedef struct typeArray {
    unsigned int size;
    unsigned short types[];
} typeArray;

typedef struct ValidTypeOperations {
    char symmetric;
   unsigned int size;
    const typeArray *general_types;
} ValidTypeOperations;

// const ValidTypeOperations valid_type_operations[] = {
// 	[PLUS] = (struct ValidTypeOperations){.symmetric = true, .size = 1, .general_types = (typeArray[]){(typeArray){3, NUMERIC, ALPHABETICAL, POINTER}}},
// 	[MINUS] = (struct ValidTypeOperations){.symmetric = true, .size = 1, .general_types = (typeArray[]){(typeArray){3, NUMERIC, ALPHABETICAL, POINTER}}},
// 	[MULT] = (struct ValidTypeOperations){.symmetric = true, .size = 1, .general_types = (typeArray[]){(typeArray){1, NUMERIC}}}
// };

#endif
