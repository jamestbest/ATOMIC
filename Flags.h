//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_FLAGS_H
#define ATOMIC_FLAGS_H

#include "Commons.h"

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
    ATOM_CT__FLAG_EXPR_DBG,
    ATOM_CT__FLAG_VEXPR_DBG,
    ATOM_CT__FLAG_TOK_OUT,
    ATOM_CT__FLAG_VTOK_OUT,
    ATOM_CT__FLAG_VLTOK_OUT,
    ATOM_CT__FLAG_AST_OUT,
    ATOM_CT__FLAG_FLAGS_OUT,
    ATOM_CT__FLAG_SCOPE_OUT,
    ATOM_CT__FLAG_HADRON_EXPAND,
    ATOM_CT__FLAG_COUNT,
};
//%%END%%

//%%OPTION DEFINE%%
#define ATOM_CT__OPTION_E_HASH 0x45
#define ATOM_CT__OPTION_O_HASH 0x4f
#define ATOM_CT__OPTION_OUT_HASH 0x54554f
//%%END%%

//%%FLAG DEFINE%% FLAG PREPROCESSOR GENERATED DEFINES SHOULD APPEAR HERE
#define ATOM_CT__FLAG_EXPR_DBG_HASH 0x4742440052505845
#define ATOM_CT__FLAG_VEXPR_DBG_HASH 0x4244005250584556
#define ATOM_CT__FLAG_TOK_OUT_HASH 0x54554f004b4f54
#define ATOM_CT__FLAG_VTOK_OUT_HASH 0x54554f004b4f5456
#define ATOM_CT__FLAG_VLTOK_OUT_HASH 0x554f004b4f544c56
#define ATOM_CT__FLAG_AST_OUT_HASH 0x54554f00545341
#define ATOM_CT__FLAG_FLAGS_OUT_HASH 0x554f005347414c46
#define ATOM_CT__FLAG_SCOPE_OUT_HASH 0x554f0045504f4353
#define ATOM_CT__FLAG_HADRON_EXPAND_HASH 0x45004e4f52444148
//%%END%%

//%%FLAG STRINGS%%
#define ATOM_CT__FLAG_EXPR_DBG_STR "EXPR_DBG"
#define ATOM_CT__FLAG_VEXPR_DBG_STR "VEXPR_DBG"
#define ATOM_CT__FLAG_TOK_OUT_STR "TOK_OUT"
#define ATOM_CT__FLAG_VTOK_OUT_STR "VTOK_OUT"
#define ATOM_CT__FLAG_VLTOK_OUT_STR "VLTOK_OUT"
#define ATOM_CT__FLAG_AST_OUT_STR "AST_OUT"
#define ATOM_CT__FLAG_FLAGS_OUT_STR "FLAGS_OUT"
#define ATOM_CT__FLAG_SCOPE_OUT_STR "SCOPE_OUT"
#define ATOM_CT__FLAG_HADRON_EXPAND_STR "HADRON_EXPAND"
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
