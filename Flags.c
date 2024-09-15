//
// Created by jamescoward on 08/11/2023.
//

#include "Flags.h"

typedef struct FlagInfo {
    const char* input_string;
    bool default_value;
} FlagInfo;

const FlagInfo flag_info[10] = {
    [0] = (FlagInfo){.input_string = "EXPR-DBG", .default_value = false}
};

bool ATOM_VR__FLAGS[ATOM_CT__FLAG_COUNT];

int flag_int_to_index(long long int fi) {
    switch (fi) {
        //%%FLAG INDEX SWITCH%% FLAG PREPROCESSOR GENERATED SWITCH CASE SHOULD APPEAR HERE
		case ATOM_CT__FLAG_EXPR_DBG_HASH:
			return ATOM_CT__FLAG_EXPR_DBG;
		case ATOM_CT__FLAG_VEXPR_DBG_HASH:
			return ATOM_CT__FLAG_VEXPR_DBG;
		case ATOM_CT__FLAG_TOK_OUT_HASH:
			return ATOM_CT__FLAG_TOK_OUT;
		case ATOM_CT__FLAG_VTOK_OUT_HASH:
			return ATOM_CT__FLAG_VTOK_OUT;
		case ATOM_CT__FLAG_VLTOK_OUT_HASH:
			return ATOM_CT__FLAG_VLTOK_OUT;
		case ATOM_CT__FLAG_AST_OUT_HASH:
			return ATOM_CT__FLAG_AST_OUT;
		case ATOM_CT__FLAG_FLAGS_OUT_HASH:
			return ATOM_CT__FLAG_FLAGS_OUT;
		case ATOM_CT__FLAG_SCOPE_OUT_HASH:
			return ATOM_CT__FLAG_SCOPE_OUT;
		case ATOM_CT__FLAG_HADRON_VERIFY_HASH:
			return ATOM_CT__FLAG_HADRON_VERIFY;
		default:
			return -1;
        //%%END%%
    }
}

char* flag_index_to_string(int index) {
    switch(index) {
        //%%FLAG STR SWITCH%%
		case ATOM_CT__FLAG_EXPR_DBG:
			return ATOM_CT__FLAG_EXPR_DBG_STR;
		case ATOM_CT__FLAG_VEXPR_DBG:
			return ATOM_CT__FLAG_VEXPR_DBG_STR;
		case ATOM_CT__FLAG_TOK_OUT:
			return ATOM_CT__FLAG_TOK_OUT_STR;
		case ATOM_CT__FLAG_VTOK_OUT:
			return ATOM_CT__FLAG_VTOK_OUT_STR;
		case ATOM_CT__FLAG_VLTOK_OUT:
			return ATOM_CT__FLAG_VLTOK_OUT_STR;
		case ATOM_CT__FLAG_AST_OUT:
			return ATOM_CT__FLAG_AST_OUT_STR;
		case ATOM_CT__FLAG_FLAGS_OUT:
			return ATOM_CT__FLAG_FLAGS_OUT_STR;
		case ATOM_CT__FLAG_SCOPE_OUT:
			return ATOM_CT__FLAG_SCOPE_OUT_STR;
		case ATOM_CT__FLAG_HADRON_VERIFY:
			return ATOM_CT__FLAG_HADRON_VERIFY_STR;
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

bool flag_set_from_idx(int index, bool enable) {
    if (index == -1) {
        return false;
    }

    ATOM_VR__FLAGS[index] = enable;
    return true;
}

bool flag_get(enum ATOM_CT__FLAGS flag) {
    return ATOM_VR__FLAGS[flag];
}

void print_flags(void) {
    printf("\nFLAGS\n");
    for (int i = 0; i < ATOM_CT__FLAG_COUNT - 1; i++) {
        printf("|-%s : %s" C_RST "\n", flag_index_to_string(i), flag_get(i) == 1 ? C_GRN "True" : C_RED "False");
    }
    printf("`-%s : %s" C_RST "\n\n", flag_index_to_string(ATOM_CT__FLAG_COUNT - 1), flag_get(ATOM_CT__FLAG_COUNT - 1) == 1 ? C_GRN "True" : C_RED "False");
}
