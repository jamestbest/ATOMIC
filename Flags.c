// This is a string dawdad adwad wada D  Dwad awda dwadadwawadwadwsadwa
// Created by jamescoward on 08/11/2023.
//

#include "Flags.h"

//%%OPTION ENUM%%
enum ATOM_CT__OPTIONS {
    ATOM_CT__OPTION_E,
    ATOM_CT__OPTION_O,
    ATOM_CT__OPTION_OUT,
};
//%%END%%

//%%FLAG ENUM%%
enum ATOM_CT__FLAGS {
    ATOM_CT__FLAG_TOK_OUT,
    ATOM_CT__FLAG_AST_OUT,
    ATOM_CT__FLAG_TEST,
    ATOM_CT__FLAG_ANOTHER_FLAG,
    ATOM_CT__FLAG_TEST_FLAG,
    ATOM_CT__FLAG_TEST_FOUR,
    ATOM_CT__FLAG_COUNT,
};
//%%END%%

bool ATOM_VR__FLAGS[ATOM_CT__FLAG_COUNT];

int flag_int_to_index(long long int fi) {
    switch (fi) {
        //%%FLAG INDEX SWITCH%% FLAG PREPROCESSOR GENERATED SWITCH CASE SHOULD APPEAR HERE
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
		case ATOM_CT__FLAG_TEST_FOUR_HASH:
			return ATOM_CT__FLAG_TEST_FOUR;
		default:
			return -1;
        //%%END%%
    }
}

char* flag_index_to_string(int index) {
    switch(index) {
        //%%FLAG STR SWITCH%%
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
		case ATOM_CT__FLAG_TEST_FOUR:
			return ATOM_CT__FLAG_TEST_FOUR_STR;
		default:
			return "ERROR NO ENUM NAME";
        //%%END%%
    }
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
