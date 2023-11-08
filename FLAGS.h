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

/* [[NOTE]]
 * Must have the options enums and defines before the flags as they share the same internal buffer for the enums
 */

//%%OPTION ENUM%%
enum ATOM_CT__OPTIONS {
    ATOM_CT__OPTION_E,
    ATOM_CT__OPTION_O,
    ATOM_CT__OPTION_OUT,
    //%%END%%
};

//%%OPTION DEFINE%%
#define ATOM_CT__OPTION_E_HASH 0x45
#define ATOM_CT__OPTION_O_HASH 0x4f
#define ATOM_CT__OPTION_OUT_HASH 0x54554f
//%%END%%

//%%FLAG ENUM%%
enum ATOM_CT__FLAGS {
    ATOM_CT__FLAG_TOK_OUT,
    ATOM_CT__FLAG_AST_OUT,
    ATOM_CT__FLAG_TEST,
    ATOM_CT__FLAG_ANOTHER_FLAG,
    ATOM_CT__FLAG_TEST_FLAG,
    ATOM_CT__FLAG_TEST_TWO_FLAG,
    //%%END%%
    ATOM_CT__FLAG_COUNT,
};

//%%FLAG DEFINE%% FLAG PREPROCESSOR GENERATED DEFINES SHOULD APPEAR HERE
#define ATOM_CT__FLAG_TOK_OUT_HASH 0x54554f0d4b4f54
#define ATOM_CT__FLAG_AST_OUT_HASH 0x54554f0d545341
#define ATOM_CT__FLAG_TEST_HASH 0x54534554
#define ATOM_CT__FLAG_ANOTHER_FLAG_HASH 0xd524548544f4e41
#define ATOM_CT__FLAG_TEST_FLAG_HASH 0x414c460d54534554
#define ATOM_CT__FLAG_TEST_TWO_FLAG_HASH 0x4f57540d54534554
//%%END%%

//%%FLAG STRINGS%%
#define ATOM_CT__FLAG_TOK_OUT_STR "TOK_OUT"
#define ATOM_CT__FLAG_AST_OUT_STR "AST_OUT"
#define ATOM_CT__FLAG_TEST_STR "TEST"
#define ATOM_CT__FLAG_ANOTHER_FLAG_STR "ANOTHER_FLAG"
#define ATOM_CT__FLAG_TEST_FLAG_STR "TEST_FLAG"
#define ATOM_CT__FLAG_TEST_TWO_FLAG_STR "TEST_TWO_FLAG"
//%%END%%

bool ATOM_VR__FLAGS[ATOM_CT__FLAG_COUNT];

int flag_int_to_index(long long int fi) {
    //%%FLAG INDEX SWITCH%% FLAG PREPROCESSOR GENERATED SWITCH CASE SHOULD APPEAR HERE
	switch (fi) {
		case ATOM_CT__FLAG_TOK_OUT_HASH:
			return ATOM_CT__FLAG_TOK_OUT;
		case ATOM_CT__FLAG_AST_OUT_HASH:
			return ATOM_CT__FLAG_AST_OUT;
		case ATOM_CT__FLAG_TEST_HASH:
			return ATOM_CT__FLAG_TEST;
		case ATOM_CT__FLAG_ANOTHER_FLAG_HASH:
			return ATOM_CT__FLAG_ANOTHER_FLAG;
		case ATOM_CT__FLAG_TEST_FLAG_HASH:
			return ATOM_CT__FLAG_TEST_FLAG;
		case ATOM_CT__FLAG_TEST_TWO_FLAG_HASH:
			return ATOM_CT__FLAG_TEST_TWO_FLAG;
		default:
			return -1;
	}
    //%%END%%
}

char* flag_index_to_string(int index) {
    //%%FLAG STR SWITCH%%
	switch(index) {
		case ATOM_CT__FLAG_TOK_OUT:
			return ATOM_CT__FLAG_TOK_OUT_STR;
		case ATOM_CT__FLAG_AST_OUT:
			return ATOM_CT__FLAG_AST_OUT_STR;
		case ATOM_CT__FLAG_TEST:
			return ATOM_CT__FLAG_TEST_STR;
		case ATOM_CT__FLAG_ANOTHER_FLAG:
			return ATOM_CT__FLAG_ANOTHER_FLAG_STR;
		case ATOM_CT__FLAG_TEST_FLAG:
			return ATOM_CT__FLAG_TEST_FLAG_STR;
		case ATOM_CT__FLAG_TEST_TWO_FLAG:
			return ATOM_CT__FLAG_TEST_TWO_FLAG_STR;
		default:
			return "ERROR NO ENUM NAME";
	}
    //%%END%%
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

bool flag_get(enum ATOM_CT__FLAGS flag) {
    return ATOM_VR__FLAGS[flag];
}

void print_flags() {
    printf("\nFLAGS\n");
    for (int i = 0; i < ATOM_CT__FLAG_COUNT - 1; i++) {
        printf("|-%s : %s" C_RST "\n", flag_index_to_string(i), flag_get(i) == 1 ? C_GRN "True" : C_RED "False");
    }
    printf("`-%s : %s" C_RST "\n", flag_index_to_string(ATOM_CT__FLAG_COUNT - 1), flag_get(ATOM_CT__FLAG_COUNT - 1) == 1 ? C_GRN "True" : C_RED "False");
}

#endif //ATOMIC_FLAGS_H