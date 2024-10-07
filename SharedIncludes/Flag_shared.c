//
// Created by jamescoward on 08/11/2023.
//

#include "Flag_shared.h"

#include "../Flags.h"
#include "Colours.h"

void flag_get_check_index(const size_t index) {
    if (index >= ATOM_CT__FLAG_COUNT) {
        printf("Accessing flag info out of range: `%zu` OOR for size `%int`", index, ATOM_CT__FLAG_COUNT);
        assert(false);
    }
}

bool flag_get_value(const size_t index) {
    flag_get_check_index(index);
    return ATOM_VR__FLAGS[index];
}

FlagInfo flag_get_info(const size_t index) {
    flag_get_check_index(index);
    return ATOM_CT__FLAGINFO[index];
}

const char* flag_get_str(const size_t index) {
    flag_get_check_index(index);
    return ATOM_CT__FLAGINFO[index].flag_name;
}

void print_flags(void) {
    printf("\nFLAGS\n");
    for (int i = 0; i < ATOM_CT__FLAG_COUNT - 1; i++) {
        printf("|-%s : %s" C_RST "\n", flag_get_str(i), flag_get_value(i) == 1 ? C_GRN "True" : C_RED "False");
    }
    printf("`-%s : %s" C_RST "\n\n", flag_get_str(ATOM_CT__FLAG_COUNT - 1), flag_get_value(ATOM_CT__FLAG_COUNT - 1) == 1 ? C_GRN "True" : C_RED "False");
}
