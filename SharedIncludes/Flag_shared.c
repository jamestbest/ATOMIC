//
// Created by jamescoward on 08/11/2023.
//

#include "Flag_shared.h"

#include "../Flags.h"
#include "Colours.h"

#include <string.h>
#include <stdlib.h>

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

void flag_set(const size_t index, const bool value) {
    flag_get_check_index(index);
    ATOM_VR__FLAGS[index] = value;
}

int compare_flags(const void* a, const void* b) {
    const FlagInfo* fa = a;
    const FlagInfo* fb = b;

    return strcmp(fa->flag_name, fb->flag_name);
}

int compare_str_to_flag(const void* str, const void* flag) {
    return strcasecmp(str, ((FlagInfo*)flag)->flag_name);
}

size_t flag_find(const char* name) {
    FlagInfo* loc = bsearch(
        name,
        ATOM_CT__FLAGINFO,
        ATOM_CT__FLAG_COUNT,
        sizeof (ATOM_CT__FLAGINFO[0]),
        compare_str_to_flag
    );

    if (!loc) {
        return -1;
    }

    return loc - ATOM_CT__FLAGINFO;
}

int compare_str_to_option(const void* str, const void* option) {
    return strcasecmp(str, ((OptionInfo*)option)->option_name);
}

size_t option_find(const char* name) {
    StaticOptionInfo* loc = bsearch(
        name,
        ATOM_CT__OPTIONINFO,
        ATOM_CT__OPTION_COUNT,
        sizeof (ATOM_CT__OPTIONINFO[0]),
        compare_str_to_option
    );

    if (!loc) {
        return -1;
    }

    return loc - ATOM_CT__OPTIONINFO;
}

void print_flags(void) {
    printf("\nFLAGS\n");
    for (int i = 0; i < ATOM_CT__FLAG_COUNT - 1; i++) {
        printf("|-%s : %s" C_RST "\n", flag_get_str(i), flag_get_value(i) == 1 ? C_GRN "True" : C_RED "False");
    }
    printf("`-%s : %s" C_RST "\n\n", flag_get_str(ATOM_CT__FLAG_COUNT - 1), flag_get_value(ATOM_CT__FLAG_COUNT - 1) == 1 ? C_GRN "True" : C_RED "False");
}
