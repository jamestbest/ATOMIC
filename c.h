//
//  CREATED BY TypePreprocessor on 07/23/24 18:05:41.c
//

#ifndef c_H
#define c_H

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

const ValidTypeOperations valid_type_operations[] = {
	[PLUS] = (struct ValidTypeOperations){.symmetric = false, .size = 2, .general_types = (typeArray[]){(typeArray){3, NUMERIC, ALPHABETICAL, POINTER}, (typeArray){1, NUMERIC}}},
	[MINUS] = (struct ValidTypeOperations){.symmetric = true, .size = 1, .general_types = (typeArray[]){(typeArray){3, NUMERIC, ALPHABETICAL, POINTER}}},
	[MULT] = (struct ValidTypeOperations){.symmetric = true, .size = 1, .general_types = (typeArray[]){(typeArray){1, NUMERIC}}}
};

#endif
