#define TYPE_COUNT 21
#define GTYPE_COUNT 11
#define TYPEFIX_COUNT 1

typedef struct OpInfo {
    uint8_t precedence: 5;
    uint8_t assoc: 1;
    uint8_t type: 2;
} OpInfo;

typedef enum ATOM_CT__LEX_TYPES_GENERAL_ENUM {
	GTYPE_POINTER,
	GTYPE_BOOLEAN,
	GTYPE_CHAR,
	GTYPE_FUNCTION,
	GTYPE_INTEGER,
	GTYPE_NATURAL,
	GTYPE_RATIONAL,
	GTYPE_REAL,
	GTYPE_STRING,
	GTYPE_STRUCT,
	GTYPE_VOID
} ATOM_CT__LEX_TYPES_GENERAL_ENUM;

typedef enum ATOM_CT__LEX_TYPES_ENUM {
	TYPE_BOOL, // BOOLEAN
	TYPE_CHR, // CHAR
	TYPE_FUNC, TYPE_PROC, // FUNCTION
	TYPE_I1, TYPE_I2, TYPE_I4, TYPE_I8, // INTEGER
	TYPE_N1, TYPE_N2, TYPE_N4, TYPE_N8, // NATURAL
	TYPE_Q4, TYPE_Q8, TYPE_Q16, // RATIONAL
	TYPE_R4, TYPE_R8, TYPE_R10, // REAL
	TYPE_STR, // STRING
	TYPE_STRUCT, // STRUCT
	TYPE_VOID // VOID
} ATOM_CT__LEX_TYPES_ENUM;

typedef enum ATOM_CT__LEX_OPERATORS_ENUM {
	OP_AMPERSAND ,  // &
	OP_ARROW     ,  // ->
	OP_ASSIGN    ,  // =
	OP_ASS_BAND  ,  // &=
	OP_ASS_BOR   ,  // |=
	OP_ASS_DIV   ,  // /=
	OP_ASS_MINUS ,  // -=
	OP_ASS_MOD   ,  // %=
	OP_ASS_MULT  ,  // *=
	OP_ASS_PLUS  ,  // +=
	OP_ASS_POW   ,  // ^=
	OP_ASS_SHL   ,  // <<=
	OP_ASS_SHR   ,  // >>=
	OP_BAND      ,  // &
	OP_BNOT      ,  // ~
	OP_BOR       ,  // |
	OP_BXOR      ,  // |-
	OP_DEC       ,  // --
	OP_DEREF     ,  // *
	OP_DIV       ,  // /
	OP_EQU       ,  // ==
	OP_INC       ,  // ++
	OP_LAND      ,  // &&
	OP_LESS      ,  // <
	OP_LESSEQ    ,  // <=
	OP_LNOT      ,  // !
	OP_LOR       ,  // ||
	OP_LXOR      ,  // |-|
	OP_MINUS     ,  // -
	OP_MOD       ,  // %
	OP_MORE      ,  // >
	OP_MOREEQ    ,  // >=
	OP_MULT      ,  // *
	OP_NEQ       ,  // !=
	OP_PLUS      ,  // +
	OP_POW       ,  // ^
	OP_RANGE     ,  // ..
	OP_SHL       ,  // <<
	OP_SHR       ,  // >>
	OP_SWAP      ,  // <>
	OP_TYPE_CONV    // as
} ATOM_CT__LEX_OPERATORS_ENUM;


typedef uint8_t* TypeMatrix;

uint8_t arr[] = {
    0,0,0,0,1
};

TypeMatrix t= arr;
