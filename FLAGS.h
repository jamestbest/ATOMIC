//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_FLAGS_H
#define ATOMIC_FLAGS_H

#include <stdbool.h>
#include "SharedIncludes/helper.h"
#include "SharedIncludes/FLAG_Shared.h"

//flags
//__FLAGS[__FLAG_00] = true;
//`__` is reserved for start so `ATOM__` prefix
/*
 * ATOM_FN__  --FUNCTION
 * ATOM_CT__  --CONSTANT
 * ATOM_VR__  --VARIABLE
 */

//ATOM__FLAG_ is a prefix
//THE NEXT 8 CHARACTERS ARE USED IN THE UNIQUE HASH
//64 bit number - 8 characters in a row

/*
 * Go through each flag defined and create a struct that stores the hash and the index?
 */

//%%FLAG ENUM%%
enum ATOM__FLAGS {
    ATOM_CT__FLAG_TOK_OUT,
    ATOM_CT__FLAG_AST_OUT,
    ATOM_CT__FLAG_TEST,
    ATOM_CT__FLAG_ANOTHER_FLAG,
    //%%FLAG END%%
    ATOM_CT__FLAG_COUNT,
};

//%%FLAG DEFINE%% FLAG PREPROCESSOR GENERATED DEFINES SHOULD APPEAR HERE
#define ATOM_CT__FLAG_TOK_OUT_HASH 0x54554f0d4b4f54
#define ATOM_CT__FLAG_AST_OUT_HASH 0x54554f0d545341
#define ATOM_CT__FLAG_TEST_HASH 0x54534554
#define ATOM_CT__FLAG_ANOTHER_FLAG_HASH 0xd524548544f4e41
//%%FLAG END%%

bool ATOM_VR__FLAGS[ATOM_CT__FLAG_COUNT];

int flag_int_to_index(long long int fi) {
    //%%FLAG SWITCH%% FLAG PREPROCESSOR GENERATED SWITCH CASE SHOULD APPEAR HERE
	switch (fi) {
		case ATOM_CT__FLAG_TOK_OUT_HASH:
			 return ATOM_CT__FLAG_TOK_OUT;
		case ATOM_CT__FLAG_AST_OUT_HASH:
			 return ATOM_CT__FLAG_AST_OUT;
		case ATOM_CT__FLAG_TEST_HASH:
			 return ATOM_CT__FLAG_TEST;
		case ATOM_CT__FLAG_ANOTHER_FLAG_HASH:
			 return ATOM_CT__FLAG_ANOTHER_FLAG;
		default:
			return -1;
	}
    //%%FLAG END%%
}

int flag_to_index(char* flag) {
    return flag_int_to_index(flag_to_int(flag));
}

bool flag_set(char* flagName, bool enable) {
    int index = flag_to_index(flagName);

    if (index == -1) {
        return false;
    }

    ATOM_VR__FLAGS[index] = enable;
    return true;
}

#endif //ATOMIC_FLAGS_H