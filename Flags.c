#include "Flags.h"

const FlagInfo flag_info[ATOM_CT__FLAG_COUNT] = {
    [ATOM_CT__FLAG_AST_OUT] = (FlagInfo){.flag_name= "AST-OUT", .default_value= false},
    [ATOM_CT__FLAG_EXPR_DBG] = (FlagInfo){.flag_name= "EXPR-DBG", .default_value= false},
    [ATOM_CT__FLAG_FLAGS_OUT] = (FlagInfo){.flag_name= "FLAGS-OUT", .default_value= true},
    [ATOM_CT__FLAG_HADRON_VERIFY] = (FlagInfo){.flag_name= "HADRON-VERIFY", .default_value= false},
    [ATOM_CT__FLAG_SCOPE_OUT] = (FlagInfo){.flag_name= "SCOPE-OUT", .default_value= false},
    [ATOM_CT__FLAG_TOK_OUT] = (FlagInfo){.flag_name= "TOK-OUT", .default_value= true},
    [ATOM_CT__FLAG_VEXPR_DBG] = (FlagInfo){.flag_name= "VEXPR-DBG", .default_value= false},
    [ATOM_CT__FLAG_VLTOK_OUT] = (FlagInfo){.flag_name= "VLTOK-OUT", .default_value= false},
    [ATOM_CT__FLAG_VTOK_OUT] = (FlagInfo){.flag_name= "VTOK-OUT", .default_value= false},
};

bool ATOM_VR__FLAGS[ATOM_CT__FLAG_COUNT] = {
    false,
    false,
    true,
    false,
    false,
    true,
    false,
    false,
    false,
};

