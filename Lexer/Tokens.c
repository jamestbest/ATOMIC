//
// Created by james on 24/12/23.
//

#include "Tokens.h"
#include "TypePreprocessor/output/enum-out.h"

#include <math.h>

const char* ATOM_CT__LEX_NAV = "nav";

const char* ATOM_CT__LEX_KEYWORDS_RAW[19] = {
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
    sizeof(ATOM_CT__LEX_KEYWORDS_RAW) / sizeof (ATOM_CT__LEX_KEYWORDS_RAW[0])
};

Arr ATOM_CT__LEX_TYPES = {
    ATOM_CT__LEX_TYPES_RAW,
    sizeof(ATOM_CT__LEX_TYPES_RAW) / sizeof(ATOM_CT__LEX_TYPES_RAW[0])
};

Arr ATOM_CT__LEX_TYPES_GENERAL = {
        ATOM_CT__LEX_TYPES_GENERAL_RAW,
        sizeof(ATOM_CT__LEX_TYPES_GENERAL_RAW) / sizeof(ATOM_CT__LEX_TYPES_GENERAL_RAW[0])
};

Arr ATOM_CT__LEX_TYPES_GENERAL_SMALL = {
    ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW,
    sizeof(ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW) / sizeof(ATOM_CT__LEX_TYPES_GENERAL_SMALL_RAW[0])
};

const char* ATOM_CT__LEX_LIT_BOOLS_RAW[] = {
        "true",
        "false"
};

Arr ATOM_CT__LEX_LIT_BOOLS = {ATOM_CT__LEX_LIT_BOOLS_RAW,
        sizeof(ATOM_CT__LEX_LIT_BOOLS_RAW) / sizeof(char*)
};

Arr ATOM_CT__LEX_OPERATORS = {
        ATOM_CT__LEX_OPERATORS_RAW,
    sizeof(ATOM_CT__LEX_OPERATORS_RAW) / sizeof(char*)
};

static size_t lonp_helper(const long long unsigned int n) {
    if (n == 0) return 1;

    return floor(log10(n)) + 1;
}

uint length_of_number_printout(const long long int n) {
    return n < 0 ? lonp_helper(-n) + 1 : lonp_helper(n);
}

uint length_of_position_printout(const Position pos) {
    // I want to convert this to fprintf to NULL, but that's not possible, I could use snprintf NULL but that does not conform to the print_position format that
    //  I want to mirror for changes. So instead I leave here a comment in the hopes that future James will not forget this and make a mistake. In actuality the
    //  chances I ever look at this piece of code again are slim
    // - future james does not understand this comment but I will [[todo]] tag it
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

/*  [[INFO]]
 *  See length_of_position_printout before making changes
 */
int print_position(const Position pos) {
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
        case EXPR_BIN:
        case EXPR_UN:
        case EXPR_TRINARY:
        case EXPR_UN_PRE:
        case EXPR_UN_POST:
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
        case BACKSLASH:
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
        case EXPR_BIN:
            title = "BIN_OP";
            break;
        case EXPR_UN:
            title = "UN_OP";
            break;
        case EXPR_UN_PRE:
            title = "PRE_UN_OP";
            break;
        case EXPR_UN_POST:
            title = "POST_UN_OP";
            break;
        case EXPR_TRINARY:
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
        case BACKSLASH:
            title = "BACKSLASH";
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

        case BACKSLASH:
            putchar('\\');
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
            printf("%s (size: %u, ptr: %d)",
                   ATOM_CT__LEX_TYPES_GENERAL.arr[encoded_data.general],
                   encoded_data.size,
                   encoded_data.tf_offset
            );
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

        case EXPR_BIN:
        case EXPR_UN:
        case EXPR_UN_POST:
        case EXPR_UN_PRE:
        case EXPR_TRINARY:
            printf("%s", ATOM_CT__LEX_OPERATORS.arr[token->data.enum_pos]);
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
        default:
            assert(false);
    }
}

void print_token(const Token* token) {
    if (!token) return;

    //{<POSITION> TYPE: VALUE}
    printf("{");
    Position pos = token->pos;
    pos.end_col = token->type == NEWLINE ? 0 : pos.end_col;

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
    switch (OP_INFO[op].type) {
        case OIGT_BINARY:
            return EXPR_BIN;
        case OIGT_UNARY:
            return EXPR_UN;
        case OIGT_POSTFIX:
            return EXPR_UN_POST;
        case OIGT_PREFIX:
            return EXPR_UN_PRE;
        case OIGT_TRINARY:
            return EXPR_TRINARY;
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
        case EXPR_BIN:
        case EXPR_TRINARY:
        case EXPR_UN:
        case EXPR_UN_POST:
        case EXPR_UN_PRE:
            return true;

        default:
            return false;
    }
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
