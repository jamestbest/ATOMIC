//
// Created by james on 24/12/23.
//

#include "Tokens.h"

#include <math.h>

const char* ATOM_CT__LEX_NAV = "nav";

char* ATOM_CT__LEX_KEYWORDS_RAW[] = {
        "for", "to", "do",
        "while",
        "foreach", "in", "with",
        "times",
        "if", "elif", "else",
        "ret",
        "func", "proc", "entry",
        "cont", "brk",
        "rec", "header"
};

Arr ATOM_CT__LEX_KEYWORDS = {
        ATOM_CT__LEX_KEYWORDS_RAW,
        sizeof(ATOM_CT__LEX_KEYWORDS_RAW) / sizeof(char*)
};

char* ATOM_CT__LEX_OP_IDENTIFIERS_RAW[] = {
        "and", "or", "xor",
        "not",
        "as"
};

Arr ATOM_CT__LEX_OP_IDENTIFIERS = {
        ATOM_CT__LEX_OP_IDENTIFIERS_RAW,
        sizeof(ATOM_CT__LEX_OP_IDENTIFIERS_RAW) / sizeof(char*)
};

char* ATOM_CT__LEX_TYPES_RAW[] = {
    "i1","i2", "i4", "i8",
    "n1", "n2", "n4", "n8",
    "r4", "r8", "r10",
    "q4", "q8", "q16",
    "str", "chr",
    "bool",
};

Arr ATOM_CT__LEX_TYPES = {
    ATOM_CT__LEX_TYPES_RAW,
    sizeof(ATOM_CT__LEX_TYPES_RAW) / sizeof(char*)
};

char* ATOM_CT__LEX_TYPES_GENERAL_RAW[] = {
        "INTEGER",
        "NATURAL",
        "REAL",
        "RATIONAL",
        "STRING",
        "CHAR",
        "BOOLEAN",
        "NOT A VALUE"
};

Arr ATOM_CT__LEX_TYPES_GENERAL = {
        ATOM_CT__LEX_TYPES_GENERAL_RAW,
        sizeof(ATOM_CT__LEX_TYPES_GENERAL_RAW) / sizeof(char*)
};

char* ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW[] = {
    "i",
    "n",
    "r",
    "q",
    "str",
    "chr",
    "bool",
    "nav"
};

Arr ATOM_CT__LEX_TYPES_GENERAL_SMALL = {
        ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW,
        sizeof(ATOM_CT__LEX_TYPES_GENERAL_SMALL) / sizeof (char*)
};

char* ATOM_CT__LEX_LIT_BOOLS_RAW[] = {
        "true",
        "false"
};

Arr ATOM_CT__LEX_LIT_BOOLS = {ATOM_CT__LEX_LIT_BOOLS_RAW,
        sizeof(ATOM_CT__LEX_LIT_BOOLS_RAW) / sizeof(char*)
};

struct test {
    ATOM_CT__LEX_OPERATORS_ENUM e_val;
    const char* op_str;

};

char* ATOM_CT__LEX_OPERATORS_RAW[] = {
    "+", "-",
    "*", "/",
    "%", "^",

    "&", "|",

    "<<", ">>",

    "+=", "-=",
    "*=", "/=",
    "%=", "^=",
    "&=", "|=",
    "<<=", ">>=",

    "&&","||", "|-|",
    "|-",

    "!", "~",
    "++", "--",

    "?",

    "&",

    "as",

    "=",

    "==", "!=", "<", ">", "<=", ">=",

    "*",

    "<>",
    "..",
    "->",
};

Arr ATOM_CT__LEX_OPERATORS = {
        ATOM_CT__LEX_OPERATORS_RAW,
    sizeof(ATOM_CT__LEX_OPERATORS_RAW) / sizeof(char*)
};

static uint lonp_helper(const long long unsigned int n) {
    if (n < 1e1) return 1;
    if (n < 1e2) return 2;
    if (n < 1e3) return 3;
    if (n < 1e4) return 4;
    if (n < 1e5) return 5;
    if (n < 1e6) return 6;
    if (n < 1e7) return 7;

    return lonp_helper(n / 1e7) + 7;
}

uint length_of_number_printout(const long long int n) {
    return n < 0 ? lonp_helper(-n) + 1 : lonp_helper(n);
}

uint length_of_position_printout(Position pos) {
    uint len = 4; // '<:->'

    len += length_of_number_printout(pos.start_line);
    len += length_of_number_printout(pos.start_col);

    if (pos.end_line == (uint)-1 || pos.start_line == pos.end_line) {
        if (pos.start_col != pos.end_col) len += length_of_number_printout(pos.end_col);
        else len--;
    } else {
        len += length_of_number_printout(pos.end_col);
        len += length_of_number_printout(pos.end_line);
    }

    return len;
}

int print_position(Position pos) {
    int ret = 0;
    ret += printf("<%d:%d", pos.start_line, pos.start_col);

    if (pos.end_line == (uint)-1 || pos.start_line == pos.end_line) {
        if (pos.start_col != pos.end_col) ret += printf("-%d", pos.end_col);
    } else {
        ret += printf("-%d:%d", pos.end_line, pos.end_col);
    }

    ret += printf(">");

    return ret;
}

const char* get_token_color(TokenType type) {
    switch (type) {
        case IDENTIFIER:
            return C_BLU;
        case LIT_INT:
        case LIT_FLOAT:
        case LIT_BOOL:
        case LIT_STR:
        case LIT_CHR:
        case LIT_NAV:
            return C_CYN;
        case BRACKET_CLOSE:
        case BRACKET_OPEN:
        case PAREN_CLOSE:
        case PAREN_OPEN:
        case CURLY_CLOSE:
        case CURLY_OPEN:
            return C_GRN;
        case KEYWORD:
            return C_RED;
        case TYPE:
            return C_MGN;
        case OP_BIN:
        case OP_UN:
        case OP_TRINARY:
        case OP_BIN_OR_UN:
        case OP_UN_PRE:
        case OP_UN_POST:
        case OP_ASSIGN:
        case OP_ARITH_ASSIGN:
        case COMMA:
        case CARROT:
        case TYPE_SET:
        case TYPE_IMPL_CAST:
            return C_GRN;
        case WS_S:
        case WS_T:
        case NEWLINE:
        case COMMENT:
        case DELIMITER:
            return C_WHT;
        case TOKEN_INVALID:
        default:
            return C_RED;
    }
}

const char* get_token_type_string(TokenType type) {
    const char* title;
    switch (type) {
        case IDENTIFIER:
            title = "IDENTIFIER";
            break;
        case KEYWORD:
            title = "KEYWORD";
            break;
        case TYPE:
            title = "TYPE";
            break;
        case LIT_STR:
            title = "STR";
            break;
        case LIT_CHR:
            title = "CHR";
            break;
        case LIT_INT:
            title = "INT";
            break;
        case LIT_FLOAT:
            title = "FLOAT";
            break;
        case LIT_BOOL:
            title = "BOOL";
            break;
        case LIT_NAV:
            title = "NAV";
            break;
        case OP_ASSIGN:
            title = "OP_ASSIGN";
            break;
        case OP_ARITH_ASSIGN:
            title = "ARITHMETIC_ASSIGN";
            break;
        case OP_BIN_OR_UN:
            title = "BIN_OR_UN_OP";
            break;
        case OP_BIN:
            title = "BIN_OP";
            break;
        case OP_UN:
            title = "UN_OP";
            break;
        case OP_UN_PRE:
            title = "PRE_UN_OP";
            break;
        case OP_UN_POST:
            title = "POST_UN_OP";
            break;
        case OP_TRINARY:
            title = "TRINARY_OP";
            break;
        case BRACKET_OPEN:
        case BRACKET_CLOSE:
            title = "BRACKET";
            break;
        case CURLY_OPEN:
        case CURLY_CLOSE:
            title = "BRACE";
            break;
        case PAREN_OPEN:
        case PAREN_CLOSE:
            title = "PAREN";
            break;
        case COMMA:
            title = "COMMA";
            break;
        case CARROT:
            title = "CARROT";
            break;
        case COMMENT:
            title = "COMMENT";
            break;
        case TYPE_SET:
            title = "TYPE SET";
            break;
        case TYPE_IMPL_CAST:
            title = "TYPE INFER";
            break;
        case WS_S:
        case WS_T:
            title = "WHITESPACE";
            break;
        case NEWLINE:
            title = "NEWLINE";
            break;
        case DELIMITER:
            title = "DELIMITER";
            break;
        case TOKEN_INVALID:
            title = "INVALID";
            break;
        default:
            assert(false);
    }
    return title;
}

void print_token_type(TokenType type) {
    putz(get_token_type_string(type));
}

void print_token_type_coloured(TokenType type) {
    putz(get_token_color(type));
    print_token_type(type);
    putz(C_RST);
}

void print_token_value(const Token* token) {
    if (token == NULL) return;
    switch (token->type) {
        case BRACKET_OPEN:
            putchar('[');
            break;
        case BRACKET_CLOSE:
            putchar(']');
            break;
        case PAREN_OPEN:
            putchar('(');
            break;
        case PAREN_CLOSE:
            putchar(')');
            break;
        case CURLY_OPEN:
            putchar('{');
            break;
        case CURLY_CLOSE:
            putchar('}');
            break;

        case CARROT:
            putchar('\'');
            break;

        case LIT_INT:
            printf("%lld", token->data.integer);
            break;
        case LIT_FLOAT:
            printf("%Lf", token->data.real);
            break;

        case KEYWORD:
            printf("%s", ATOM_CT__LEX_KEYWORDS.arr[token->data.integer]);
            break;
        case TYPE: {
            const encodedType encoded_data = token->data.type;
            printf("%s (size: %d, ptr: %d)",
                   ATOM_CT__LEX_TYPES_GENERAL.arr[encoded_data.general_type],
                   encoded_data.size,
                   encoded_data.ptr_offset);
            break;
        }
        case IDENTIFIER:
        case LIT_BOOL:
            printf("%s", token->data.ptr);
            break;

        case LIT_STR:
            printf("\"%s\"", token->data.ptr);
            break;

        case LIT_CHR:
            printf("\'%s\'", token->data.ptr);
            break;

        case COMMENT:
            printf("%s", token->data.ptr);
            break;

        case LIT_NAV:
            printf("NAV");
            break;

        case OP_BIN:
        case OP_UN:
        case OP_UN_POST:
        case OP_UN_PRE:
        case OP_TRINARY:
        case OP_BIN_OR_UN:
        case OP_ARITH_ASSIGN:
            printf("%s", ATOM_CT__LEX_OPERATORS.arr[token->data.enum_pos]);
            break;

        case OP_ASSIGN:
            putchar('=');
            break;
        case COMMA:
            putchar(',');
            break;
        case TYPE_SET:
            putchar(':');
            break;
        case TYPE_IMPL_CAST:
            printf("::"); //[[todo]] there is no check for `::`
            break;
        case DELIMITER:
            printf(";");
            break;
        case WS_S:
            putchar(' ');
            break;
        case WS_T:
            putchar('\t');
            break;
        case NEWLINE:
            printf("\\n");
            break;
        case TOKEN_INVALID:
            printf("\\INVALID\\");
            break;
    }
}

void print_token(const Token* token) {
    if (!token) return;

    //{<POSITION> TYPE: VALUE}
    printf("{");
    Position pos = token->pos;
    pos.end_col = (token->type == NEWLINE) ? 0 : pos.end_col;

    print_position(pos);

    putchar(' ');

    print_token_type_coloured(token->type);

    putz(" '");
    print_token_value(token);
    printf("'}");
}

void print_token_ln(Token* token) {
    print_token(token);
    putchar('\n');
}

TokenType operator_to_type(const ATOM_CT__LEX_OPERATORS_ENUM op) {
    switch (op) {
        case PLUS:
        case MINUS:
        case MULT:
        case DEREFERENCE:
            return OP_BIN_OR_UN;

        case DIV:
        case MOD:
        case POW:
        case BAND:
        case BOR:
        case SHL:
        case SHR:
            return OP_BIN;

        case ASS_PLUS:
        case ASS_MINUS:
        case ASS_MULT:
        case ASS_DIV:
        case ASS_MOD:
        case ASS_POW:
        case ASS_BAND:
        case ASS_BOR:
        case ASS_SHL:
        case ASS_SHR:
            return OP_ARITH_ASSIGN;

        case LAND:
        case LOR:
        case LXOR:
        case BXOR:
            return OP_BIN;

        case LNOT:
        case BNOT:
        case AMPERSAND:
            return OP_UN_PRE;
        case INC:
        case DEC:
            return OP_UN;

        case QUESTION:
            return OP_TRINARY;

        case ASSIGNMENT:
            return OP_ASSIGN;

        case EQU:
        case NEQ:
        case LESS:
        case MORE:
        case LESSEQ:
        case MOREEQ:
        case SWAP:
        case RANGE:
        case ARROW:
        case TYPE_CONVERSION:
            return OP_BIN;

        default:
            return TOKEN_INVALID;
    }
}

bool type_needs_free(TokenType type) {
    switch (type) {
        case IDENTIFIER:
        case LIT_STR:
        case LIT_CHR:
        case LIT_BOOL:
        case COMMENT:
            return true;
        default:
            return false;
    }
}

bool is_whitespace_tkn(TokenType type) {
    return type == WS_S || type == WS_T;
}

bool is_l_paren(Token* tok) {
    return tok->type == PAREN_OPEN;
}

bool is_r_paren(Token* tok) {
    return tok->type == PAREN_CLOSE;
}

bool is_l_square_bracket(Token* tok) {
    return tok->type == BRACKET_OPEN;
}

bool is_r_square_bracket(Token* tok) {
    return tok->type == BRACKET_CLOSE;
}

bool is_square_bracket(Token* tok) {
    return is_l_square_bracket(tok) || is_r_square_bracket(tok);
}

bool is_terminal(Token* tok) {
    switch (tok->type) {
        case IDENTIFIER:
        case LIT_BOOL:
        case LIT_CHR:
        case LIT_STR:
        case LIT_INT:
        case LIT_FLOAT:
        case LIT_NAV:
        case TYPE:
            return true;

        default:
            return false;
    }
}

bool is_arith_operator(Token* tok) {
    switch (tok->type) {
        case OP_BIN:
        case OP_BIN_OR_UN:
        case OP_TRINARY:
        case OP_UN:
        case OP_UN_POST:
        case OP_UN_PRE:
            return true;

        default:
            return false;
    }
}

bool is_assigning_operator(Token* tok) {
    switch (tok->type) {
        case OP_ASSIGN:
        case OP_ARITH_ASSIGN:
            return true;
        default:
            return false;
    }
}

bool is_any_operator(Token* tok) {
    return is_arith_operator(tok) || is_assigning_operator(tok);
}

void consolidate(Token* base_token, Token* token_to_eat) {
    const bool base_before = base_token->pos.start_line < token_to_eat->pos.start_line ||
            base_token->pos.start_col < token_to_eat->pos.start_col;

    if (base_before) {
        base_token->pos.end_line = token_to_eat->pos.end_line;
        base_token->pos.end_col = token_to_eat->pos.end_col;
    } else {
        base_token->pos.start_line = token_to_eat->pos.start_line;
        base_token->pos.start_col = token_to_eat->pos.start_col;
    }
}
