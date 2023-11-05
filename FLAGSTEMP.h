//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_FLAGS_H
#define ATOMIC_FLAGS_H

#include <stdbool.h>
#include "helper.h"
#include "FLAG_Shared.h"

//flags
//__FLAGS[__FLAG_00] = true;
//`__` is reserved for start so `ATOM__` prefix

//ATOM__FLAG_ is a prefix
//THE NEXT 8 CHARACTERS ARE USED IN THE UNIQUE HASH
//64 bit number - 8 characters in a row

/*
 * Go through each flag defined and create a struct that stores the hash and the index?
 */

//%%FLAG ENUM%%
enum ATOM__FLAGS {
    ATOM__FLAG_TOK_OUT,
    ATOM__FLAG_AST_OUT,
    ATOM__FLAG_,
    ATOM__FLAG_TEST,
    //%%FLAG END%%
    ATOM__FLAG_COUNT,
};

//%%FLAG DEFINE%% FLAG PREPROCESSOR GENERATED DEFINES SHOULD APPEAR HERE
#define ATOM__FLAG_TOK_OUT_HASH 0x54554f2d4b4f54
#define ATOM__FLAG_AST_OUT_HASH 0x54554f2d545341
#define ATOM__FLAG__HASH 0x0
#define ATOM__FLAG_TEST_HASH 0x54534554
//%%FLAG END%%

bool ATOM__FLAGS[ATOM__FLAG_COUNT];

#define test(NAME) case NAME.hash: \
                        return NAME.index;

int ATOM__FLAG_INT_TO_INDEX(long long int fi) {
    //%%FLAG SWITCH%% FLAG PREPROCESSOR GENERATED SWITCH CASE SHOULD APPEAR HERE
	switch (fi) {
		case ATOM__FLAG_TOK_OUT_HASH:
			 return ATOM__FLAG_TOK_OUT;
		case ATOM__FLAG_AST_OUT_HASH:
			 return ATOM__FLAG_AST_OUT;
		case ATOM__FLAG__HASH:
			 return ATOM__FLAG_;
		case ATOM__FLAG_TEST_HASH:
			 return ATOM__FLAG_TEST;
		default:
			return -1;
	}
    //%%FLAG END%%
}

int ATOM__FLAG_TO_INDEX(char* flag) {
    return ATOM__FLAG_INT_TO_INDEX(ATOM__FLAG_TO_INT(flag));
}

#endif //ATOMIC_FLAGS_H