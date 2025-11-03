#include "enum-out.h"

#include <stdint.h>

const char* ATOM_CT__LEX_TYPES_RAW[21] = {
    "bool", 
    "chr", 
    "func", 
    "i1", "i2", "i4", "i8", 
    "n1", "n2", "n4", "n8", 
    "proc", 
    "q4", "q8", "q16", 
    "r4", "r8", "r10", 
    "str", 
    "struct", 
    "void", 
};

const char* ATOM_CT__LEX_TYPES_GENERAL_RAW[]= {
    "BOOLEAN",
    "CHAR",
    "FUNCTION",
    "INTEGER",
    "NATURAL",
    "POINTER",
    "PROCEDURE",
    "RATIONAL",
    "REAL",
    "STRING",
    "STRUCT",
    "VOID" 
};

const char* ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW[]= {
    "bool",
    "chr",
    "func",
    "i",
    "n",
    "ptr",
    "proc",
    "q",
    "r",
    "str",
    "struct",
    "void" 
};

ATOM_CT__LEX_TYPES_ENUM GENERAL_TO_TYPES[GTYPE_COUNT]= {
	[GTYPE_BOOLEAN]= TYPE_BOOL,
	[GTYPE_CHAR]= TYPE_CHR,
	[GTYPE_FUNCTION]= TYPE_FUNC,
	[GTYPE_INTEGER]= TYPE_I1,
	[GTYPE_NATURAL]= TYPE_N1,
	[GTYPE_POINTER]= -1,
	[GTYPE_PROCEDURE]= TYPE_PROC,
	[GTYPE_RATIONAL]= TYPE_Q4,
	[GTYPE_REAL]= TYPE_R4,
	[GTYPE_STRING]= TYPE_STR,
	[GTYPE_STRUCT]= TYPE_STRUCT,
	[GTYPE_VOID]= TYPE_VOID 
};

ATOM_CT__LEX_TYPES_GENERAL_ENUM TYPES_TO_GENERAL[TYPE_COUNT]= {
	[TYPE_BOOL]= GTYPE_BOOLEAN,
	[TYPE_CHR]= GTYPE_CHAR,
	[TYPE_FUNC]= GTYPE_FUNCTION,
	[TYPE_I1]= GTYPE_INTEGER,
	[TYPE_I2]= GTYPE_INTEGER,
	[TYPE_I4]= GTYPE_INTEGER,
	[TYPE_I8]= GTYPE_INTEGER,
	[TYPE_N1]= GTYPE_NATURAL,
	[TYPE_N2]= GTYPE_NATURAL,
	[TYPE_N4]= GTYPE_NATURAL,
	[TYPE_N8]= GTYPE_NATURAL,
	[TYPE_PROC]= GTYPE_PROCEDURE,
	[TYPE_Q4]= GTYPE_RATIONAL,
	[TYPE_Q8]= GTYPE_RATIONAL,
	[TYPE_Q16]= GTYPE_RATIONAL,
	[TYPE_R4]= GTYPE_REAL,
	[TYPE_R8]= GTYPE_REAL,
	[TYPE_R10]= GTYPE_REAL,
	[TYPE_STR]= GTYPE_STRING,
	[TYPE_STRUCT]= GTYPE_STRUCT,
	[TYPE_VOID]= GTYPE_VOID 
};

const unsigned int FUNCTION_REQS[1]= {
   0,
};

const unsigned int INTEGER_SIZES[4]= {
   1,
   2,
   4,
   8 
};

const unsigned int NATURAL_SIZES[4]= {
   1,
   2,
   4,
   8 
};

const unsigned int PROCEDURE_REQS[1]= {
   0,
};

const unsigned int RATIONAL_SIZES[3]= {
   4,
   8,
   16 
};

const unsigned int REAL_SIZES[3]= {
   4,
   8,
   10 
};

const unsigned int VOID_REQS[1]= {
   0,
};

const TypeLike TYPE_INFO[12]= {
   [GTYPE_BOOLEAN]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=1
           },
           .has_variable_sizes= 0,
           .is_virtual= 0,
           .sizes= {.size= 0},
           .reqs= {.size= 0},
       }
   },
   [GTYPE_CHAR]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=1
           },
           .has_variable_sizes= 0,
           .is_virtual= 0,
           .sizes= {.size= 0},
           .reqs= {.size= 0},
       }
   },
   [GTYPE_FUNCTION]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 0,
           .is_virtual= 1,
           .sizes= {.size= 0},
           .reqs= (uintArrayS) {
               .size= 1,
               .arr= FUNCTION_REQS
           }
       }
   },
   [GTYPE_INTEGER]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 1,
           .is_virtual= 0,
           .sizes= (uintArrayS) {
               .size= 4,
               .arr= INTEGER_SIZES
           },
           .reqs= {.size= 0},
       }
   },
   [GTYPE_NATURAL]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 1,
           .is_virtual= 0,
           .sizes= (uintArrayS) {
               .size= 4,
               .arr= NATURAL_SIZES
           },
           .reqs= {.size= 0},
       }
   },
   [GTYPE_POINTER]=(TypeLike){
       .tf= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE_FIX,
               .size=8
           },
           .prefix= 1
       }
   },
   [GTYPE_PROCEDURE]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 0,
           .is_virtual= 1,
           .sizes= {.size= 0},
           .reqs= (uintArrayS) {
               .size= 1,
               .arr= PROCEDURE_REQS
           }
       }
   },
   [GTYPE_RATIONAL]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 1,
           .is_virtual= 0,
           .sizes= (uintArrayS) {
               .size= 3,
               .arr= RATIONAL_SIZES
           },
           .reqs= {.size= 0},
       }
   },
   [GTYPE_REAL]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 1,
           .is_virtual= 0,
           .sizes= (uintArrayS) {
               .size= 3,
               .arr= REAL_SIZES
           },
           .reqs= {.size= 0},
       }
   },
   [GTYPE_STRING]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=16
           },
           .has_variable_sizes= 0,
           .is_virtual= 0,
           .sizes= {.size= 0},
           .reqs= {.size= 0},
       }
   },
   [GTYPE_STRUCT]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 0,
           .is_virtual= 1,
           .sizes= {.size= 0},
           .reqs= {.size= 0},
       }
   },
   [GTYPE_VOID]=(TypeLike){
       .t= {
           .base= (TypeLikeInfo) {
               .type=TL_TYPE,
               .size=0
           },
           .has_variable_sizes= 0,
           .is_virtual= 0,
           .sizes= {.size= 0},
           .reqs= (uintArrayS) {
               .size= 1,
               .arr= VOID_REQS
           }
       }
   } 
};

const char* ATOM_CT__LEX_OPERATORS_RAW[]= {
    "&",
    "->",
    "=",
    "&=",
    "|=",
    "/=",
    "-=",
    "%=",
    "*=",
    "+=",
    "^=",
    "<<=",
    ">>=",
    "&",
    "~",
    "|",
    "|-",
    "--",
    "*",
    "/",
    ".",
    "==",
    "++",
    "&&",
    "<",
    "<=",
    "!",
    "||",
    "|-|",
    "-",
    "%",
    ">",
    ">=",
    "*",
    "!=",
    "+",
    "^",
    "..",
    "<<",
    ">>",
    "<>",
    "as"
};

OpInfo OP_INFO[42]= {
    [OP_AMPERSAND]=  (OpInfo){.precedence=5,.assoc=0,.type=0},
    [OP_ARROW]=      (OpInfo){5, 0, 3},
    [OP_ASSIGN]=     (OpInfo){5, 0, 3},
    [OP_ASS_BAND]=   (OpInfo){5, 0, 3},
    [OP_ASS_BOR]=    (OpInfo){5, 0, 3},
    [OP_ASS_DIV]=    (OpInfo){5, 0, 3},
    [OP_ASS_MINUS]=  (OpInfo){5, 0, 3},
    [OP_ASS_MOD]=    (OpInfo){5, 0, 3},
    [OP_ASS_MULT]=   (OpInfo){5, 0, 3},
    [OP_ASS_PLUS]=   (OpInfo){5, 0, 3},
    [OP_ASS_POW]=    (OpInfo){5, 0, 3},
    [OP_ASS_SHL]=    (OpInfo){5, 0, 3},
    [OP_ASS_SHR]=    (OpInfo){5, 0, 3},
    [OP_BAND]=       (OpInfo){5, 0, 3},
    [OP_BNOT]=       (OpInfo){5, 0, 0},
    [OP_BOR]=        (OpInfo){5, 0, 3},
    [OP_BXOR]=       (OpInfo){5, 0, 3},
    [OP_DEC]=        (OpInfo){5, 0, 2},
    [OP_DEREF]=      (OpInfo){5, 0, 0},
    [OP_DIV]=        (OpInfo){5, 0, 3},
    [OP_DOT]=        (OpInfo){5, 0, 3},
    [OP_EQU]=        (OpInfo){5, 0, 3},
    [OP_INC]=        (OpInfo){5, 0, 2},
    [OP_LAND]=       (OpInfo){5, 0, 3},
    [OP_LESS]=       (OpInfo){5, 0, 3},
    [OP_LESSEQ]=     (OpInfo){5, 0, 3},
    [OP_LNOT]=       (OpInfo){5, 0, 0},
    [OP_LOR]=        (OpInfo){5, 0, 3},
    [OP_LXOR]=       (OpInfo){5, 0, 3},
    [OP_MINUS]=      (OpInfo){5, 0, 3},
    [OP_MOD]=        (OpInfo){5, 0, 3},
    [OP_MORE]=       (OpInfo){5, 0, 3},
    [OP_MOREEQ]=     (OpInfo){5, 0, 3},
    [OP_MULT]=       (OpInfo){5, 0, 3},
    [OP_NEQ]=        (OpInfo){5, 0, 3},
    [OP_PLUS]=       (OpInfo){5, 0, 3},
    [OP_POW]=        (OpInfo){5, 0, 3},
    [OP_RANGE]=      (OpInfo){5, 0, 3},
    [OP_SHL]=        (OpInfo){5, 0, 3},
    [OP_SHR]=        (OpInfo){5, 0, 3},
    [OP_SWAP]=       (OpInfo){5, 0, 3},
    [OP_TYPE_CONV]=  (OpInfo){5, 0, 3}
};

static uint8_t COERCION_INTERNAL[18]= {
	0b10111000,
	0b10000001,
	0b00000001,
	0b00000000,
	0b00110000,
	0b00000011,
	0b00101011,
	0b10000000,
	0b00000001,
	0b00000000,
	0b00010000,
	0b00000000,
	0b00011001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

TypeMatrix COERCION_INFO= COERCION_INTERNAL;

CoercionRule COERCION_RULES[1]= {
  [0]= (CoercionRule){
    .left= {
      .is_builtin= 1,
      .idx= 3
    },
    .right= {
      .is_builtin= 0,
      .idx= 4
    } 
  } 
};

static uint8_t LESS_MATRIX[18]= {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b10000000,
	0b00000000,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00000000,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

static uint8_t LOR_MATRIX[18]= {
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b10000000,
	0b00000000,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00000000,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

static uint8_t MINUS_MATRIX[18]= {
	0b00000000,
	0b00100000,
	0b00000000,
	0b00000000,
	0b10000000,
	0b00000010,
	0b00110000,
	0b00000000,
	0b00000010,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00000000,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

static uint8_t MULT_MATRIX[18]= {
	0b00000000,
	0b10000000,
	0b00000001,
	0b00000000,
	0b10100000,
	0b00000000,
	0b00010010,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00000000,
	0b00000001,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};

static uint8_t PLUS_MATRIX[18]= {
	0b00000000,
	0b00100000,
	0b00000000,
	0b00000000,
	0b10000000,
	0b00000010,
	0b00110000,
	0b10000000,
	0b00000011,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00000000,
	0b00000001,
	0b00100000,
	0b00000000,
	0b00000000,
	0b00000000
};

OperandInfo OPERAND_INFO[42]= {
	[OP_AMPERSAND]= (OperandInfo){
		.left= {
		  .is_builtin= 1,
		  .idx= 1
		},
		.op_type= 0,
		.out_type= OUT_WRAP,
		.output_index= 0,
		.is_keyvalue= 1
	},
	[OP_ASSIGN]= (OperandInfo){
		.left= {
		  .is_builtin= 1,
		  .idx= 1
		},
		.right= {
		  .is_builtin= 1,
		  .idx= 204
		},
		.op_type= 3,
		.out_type= OUT_LEFT,
		.output_index= 608,
		.is_keyvalue= 1
	},
	[OP_DEREF]= (OperandInfo){
		.typemap=32,
		.op_type= 0,
		.out_type= OUT_UNWRAP,
		.output_index= 5,
		.is_keyvalue= 0
	},
	[OP_LESS]= (OperandInfo){
		.matrix= LESS_MATRIX,
		.op_type= 3,
		.out_type= OUT_EXPLICIT,
		.output_index= 0,
		.is_keyvalue= 0
	},
	[OP_LOR]= (OperandInfo){
		.matrix= LOR_MATRIX,
		.op_type= 3,
		.out_type= OUT_EXPLICIT,
		.output_index= 0,
		.is_keyvalue= 0
	},
	[OP_MINUS]= (OperandInfo){
		.matrix= MINUS_MATRIX,
		.op_type= 3,
		.out_type= OUT_BASED,
		.output_index= 13,
		.is_keyvalue= 0
	},
	[OP_MULT]= (OperandInfo){
		.matrix= MULT_MATRIX,
		.op_type= 3,
		.out_type= OUT_BASED,
		.output_index= 9,
		.is_keyvalue= 0
	},
	[OP_PLUS]= (OperandInfo){
		.matrix= PLUS_MATRIX,
		.op_type= 3,
		.out_type= OUT_BASED,
		.output_index= 13,
		.is_keyvalue= 0
	},
	[OP_SWAP]= (OperandInfo){
		.left= {
		  .is_builtin= 1,
		  .idx= 1
		},
		.right= {
		  .is_builtin= 1,
		  .idx= 1
		},
		.op_type= 3,
		.out_type= OUT_EXPLICIT,
		.output_index= 11,
		.is_keyvalue= 1
	},
	[OP_TYPE_CONV]= (OperandInfo){
		.left= {
		  .is_builtin= 1,
		  .idx= 2
		},
		.right= {
		  .is_builtin= 1,
		  .idx= 3
		},
		.op_type= 3,
		.out_type= OUT_RIGHT,
		.output_index= 608,
		.is_keyvalue= 1
	} 
};

