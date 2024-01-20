//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_FLAGS_H
#define ATOMIC_FLAGS_H

#include <stdbool.h>
#include <stdio.h>
#include "SharedIncludes/Helper_Math.h"
#include "SharedIncludes/Flag_shared.h"
#include "SharedIncludes/Colours.h"

//flags
//__FLAGS[__FLAG_00] = true;
//`__` is reserved for start so `ATOM_XX__` prefix
/*
 * ATOM_FN__  --FUNCTION
 * ATOM_CT__  --CONSTANT
 * ATOM_VR__  --VARIABLE
 */

//ATOM__FLAG_ is a prefix
//THE NEXT 8 CHARACTERS (BAR '-') ARE USED IN THE UNIQUE HASH
//64 bit number - 8 characters in a row

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
    ATOM_CT__FLAG_VTOK_OUT,
    ATOM_CT__FLAG_VLTOK_OUT,
    ATOM_CT__FLAG_AST_OUT,
    ATOM_CT__FLAG_TESTER_OUT,
    ATOM_CT__FLAG_TEST,
    ATOM_CT__FLAG_ANOTHER_FLAG,
    ATOM_CT__FLAG_TEST_FLAG,
    ATOM_CT__FLAG_TEST_FOUR,
    ATOM_CT__FLAG_COUNT,
};
//%%END%%

//%%OPTION DEFINE%%
#define ATOM_CT__OPTION_E_HASH 0x45
#define ATOM_CT__OPTION_O_HASH 0x4f
#define ATOM_CT__OPTION_OUT_HASH 0x54554f
//%%END%%

//%%FLAG DEFINE%% FLAG PREPROCESSOR GENERATED DEFINES SHOULD APPEAR HERE
#define ATOM_CT__FLAG_TOK_OUT_HASH 0x54554f004b4f54
#define ATOM_CT__FLAG_VTOK_OUT_HASH 0x54554f004b4f5456
#define ATOM_CT__FLAG_VLTOK_OUT_HASH 0x554f004b4f544c56
#define ATOM_CT__FLAG_AST_OUT_HASH 0x54554f00545341
#define ATOM_CT__FLAG_TESTER_OUT_HASH 0x4f00524554534555
#define ATOM_CT__FLAG_TEST_HASH 0x54534554
#define ATOM_CT__FLAG_ANOTHER_FLAG_HASH 0x524548544f4e47
#define ATOM_CT__FLAG_TEST_FLAG_HASH 0x414c460054534557
#define ATOM_CT__FLAG_TEST_FOUR_HASH 0x554f460054534556
//%%END%%

//%%FLAG STRINGS%%
#define ATOM_CT__FLAG_TOK_OUT_STR "TOK_OUT"
#define ATOM_CT__FLAG_VTOK_OUT_STR "VTOK_OUT"
#define ATOM_CT__FLAG_VLTOK_OUT_STR "VLTOK_OUT"
#define ATOM_CT__FLAG_AST_OUT_STR "AST_OUT"
#define ATOM_CT__FLAG_TESTER_OUT_STR "TESTER_OUT"
#define ATOM_CT__FLAG_TEST_STR "TEST"
#define ATOM_CT__FLAG_ANOTHER_FLAG_STR "ANOTHER_FLAG"
#define ATOM_CT__FLAG_TEST_FLAG_STR "TEST_FLAG"
#define ATOM_CT__FLAG_TEST_FOUR_STR "TEST_FOUR"
//%%END%%

//%%OPTION STRINGS%%
#define ATOM_CT__OPTION_E_STR "E"
#define ATOM_CT__OPTION_O_STR "O"
#define ATOM_CT__OPTION_OUT_STR "OUT"
//%%END%%

int flag_int_to_index(long long int fi);
char* flag_index_to_string(int index);

void print_flags(void);
bool flag_get(enum ATOM_CT__FLAGS flag);
bool flag_set(char* flagName, bool enable);
bool flag_set_from_idx(int index, bool enable);
int flag_to_index(char* flag);

#endif //ATOMIC_FLAGS_H
