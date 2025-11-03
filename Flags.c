#include "Flags.h"

const FlagInfo ATOM_CT__FLAGINFO[ATOM_CT__FLAG_COUNT] = {
    [ATOM_CT__FLAG_AST_OUT] = (FlagInfo){.flag_name= "AST-OUT", .default_value= false},
    [ATOM_CT__FLAG_BYTE_OUT] = (FlagInfo){.flag_name= "BYTE-OUT", .default_value= false},
    [ATOM_CT__FLAG_EXPR_DBG] = (FlagInfo){.flag_name= "EXPR-DBG", .default_value= false},
    [ATOM_CT__FLAG_FLAGS_OUT] = (FlagInfo){.flag_name= "FLAGS-OUT", .default_value= false},
    [ATOM_CT__FLAG_HADRON_VERIFY] = (FlagInfo){.flag_name= "HADRON-VERIFY", .default_value= true},
    [ATOM_CT__FLAG_SCOPE_OUT] = (FlagInfo){.flag_name= "SCOPE-OUT", .default_value= false},
    [ATOM_CT__FLAG_TOK_OUT] = (FlagInfo){.flag_name= "TOK-OUT", .default_value= false},
    [ATOM_CT__FLAG_VEXPR_DBG] = (FlagInfo){.flag_name= "VEXPR-DBG", .default_value= false},
    [ATOM_CT__FLAG_VLTOK_OUT] = (FlagInfo){.flag_name= "VLTOK-OUT", .default_value= false},
    [ATOM_CT__FLAG_VTOK_OUT] = (FlagInfo){.flag_name= "VTOK-OUT", .default_value= false},
};

bool ATOM_VR__FLAGS[ATOM_CT__FLAG_COUNT] = {
    false,
    false,
    false,
    false,
    true,
    false,
    false,
    false,
    false,
    false,
};

const StaticOptionInfo ATOM_CT__OPTIONINFO[ATOM_CT__OPTION_COUNT] = {
	[ATOM_CT__OPTION_BO]= (StaticOptionInfo) {
		.option_name= "BO",
		.arg_info_count= 1,
		.args= (StaticOptionArgInfo[]) {
			{
				.arg_name= "OUTPUT-FILE",
				.repeated= false,
				.type= FP_TYPE_STR,
				.arg_option_count= 0,
				.data= (OptionData[]) {
				},
			},
		},
	},
	[ATOM_CT__OPTION_DEBUGLVL]= (StaticOptionInfo) {
		.option_name= "DEBUGLVL",
		.arg_info_count= 1,
		.args= (StaticOptionArgInfo[]) {
			{
				.arg_name= "LVL",
				.repeated= false,
				.type= FP_TYPE_NATURAL,
				.arg_option_count= 3,
				.data= (OptionData[]) {
					{.natural= 0},
					{.natural= 1},
					{.natural= 2},
				},
			},
		},
	},
	[ATOM_CT__OPTION_E]= (StaticOptionInfo) {
		.option_name= "E",
		.arg_info_count= 1,
		.args= (StaticOptionArgInfo[]) {
			{
				.arg_name= "Entry",
				.repeated= false,
				.type= FP_TYPE_STR,
				.arg_option_count= 0,
				.data= (OptionData[]) {
				},
			},
		},
	},
	[ATOM_CT__OPTION_O]= (StaticOptionInfo) {
		.option_name= "O",
		.arg_info_count= 1,
		.args= (StaticOptionArgInfo[]) {
			{
				.arg_name= "OUTPUT-FORMAT",
				.repeated= false,
				.type= FP_TYPE_STR,
				.arg_option_count= 0,
				.data= (OptionData[]) {
				},
			},
		},
	},
	[ATOM_CT__OPTION_OUT]= (StaticOptionInfo) {
		.option_name= "OUT",
		.arg_info_count= 1,
		.args= (StaticOptionArgInfo[]) {
			{
				.arg_name= "OUTPUT",
				.repeated= true,
				.type= FP_TYPE_STR,
				.arg_option_count= 7,
				.data= (OptionData[]) {
					{.str= "AST"},
					{.str= "BYTE"},
					{.str= "FLAGS"},
					{.str= "SCOPE"},
					{.str= "TOK"},
					{.str= "VLTOK"},
					{.str= "VTOK"},
				},
			},
		},
	},
	[ATOM_CT__OPTION_TEST]= (StaticOptionInfo) {
		.option_name= "TEST",
		.arg_info_count= 3,
		.args= (StaticOptionArgInfo[]) {
			{
				.arg_name= "INP1-FOR",
				.repeated= false,
				.type= FP_TYPE_STR,
				.arg_option_count= 0,
				.data= (OptionData[]) {
				},
			},
			{
				.arg_name= "INP2-FOR",
				.repeated= false,
				.type= FP_TYPE_NATURAL,
				.arg_option_count= 3,
				.data= (OptionData[]) {
					{.natural= 0},
					{.natural= 5},
					{.natural= 19},
				},
			},
			{
				.arg_name= "INP3-FOR",
				.repeated= false,
				.type= FP_TYPE_CHARACTER,
				.arg_option_count= 3,
				.data= (OptionData[]) {
					{.character= 'a'},
					{.character= 'b'},
					{.character= 'c'},
				},
			},
		},
	},
};

