//
// Created by james on 24/12/23.
//

#include "Tokens.h"

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
        "not"
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
    "nav",
};

Arr ATOM_CT__LEX_TYPES = {
    ATOM_CT__LEX_TYPES_RAW,
    sizeof(ATOM_CT__LEX_TYPES_RAW) / sizeof(char*)
};

char* ATOM_CT__LEX_CONS_IDENTIFIERS_RAW[] = {
        "true",
        "false"
};

Arr ATOM_CT__LEX_CONS_IDENTIFIERS = {
        ATOM_CT__LEX_CONS_IDENTIFIERS_RAW,
        sizeof(ATOM_CT__LEX_CONS_IDENTIFIERS_RAW) / sizeof(char*)
};

int print_position(Position pos) {
    int ret = 0;
    ret += printf("<%d:%d", pos.start_line, pos.start_col);

    if (pos.end_line == (uint)-1 || pos.start_line == pos.end_line) {
        if (pos.start_col != pos.end_col) ret += printf("-%d", pos.end_col);
    } else {
        ret += printf("-%d:%d", pos.end_line, pos.end_col);
    }

    ret += printf("> ");

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
        case OP_UN_PRE:
        case OP_UN_POST:
        case OP_TRINARY:
        case COMMA:
        case TYPE_SET:
        case TYPE_INFER:
            return C_GRN;
        case WS_S:
        case WS_T:
        case NEWLINE:
        case COMMENT:
        case DELIMITER:
            return C_WHT;
        case EOTS:
            return C_RED;
    }
}

//this is dynamically allocated memory so be sure to free
const char* cons_token_type_colored(TokenType type) {
    const char* colour = get_token_color(type);
    const char* title;
    const char* end = C_RST;
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
        case OP_BIN:
            title = "BIN_OP";
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
        case COMMENT:
            title = "COMMENT";
            break;
        case TYPE_SET:
            title = "TYPE SET";
            break;
        case TYPE_INFER:
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
        case EOTS:
            title = "END OF TOKEN STREAM";
            break;
    }
    uint length = len(colour) + len(title) + len(end) + 1;
    char* ret = malloc(sizeof(char) * length);
    snprintf(ret, length, "%s%s%s", colour, title, end);

    return ret;
}

void print_token_value(Token* token) {
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

        case LIT_INT:
            printf("%ld", *(int64_t*)token->data);
            break;
        case LIT_FLOAT:
            printf("%Lf", *(long double*)token->data);
            break;

        case IDENTIFIER:
        case KEYWORD:
        case TYPE:
        case LIT_BOOL:
            printf("%s", (char*)token->data);
            break;

        case LIT_STR:
            printf("\"%s\"", (char*)token->data);
            break;

        case COMMENT:
            printf("%s", (char*)token->data);
            break;

        case LIT_NAV:
            printf("NAV");
            break;

        case OP_BIN:
        case OP_UN_PRE:
        case OP_UN_POST:
        case OP_TRINARY:
            printf("%s", (char*)token->data);
            break;

        case COMMA:
            putchar(',');
            break;
        case TYPE_SET:
            putchar(':');
            break;
        case TYPE_INFER:
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
        case EOTS:
            printf("\\EOTS\\");
            break;
    }
}

void print_token(Token* token) {
    //{<POSITION> TYPE: VALUE}
    printf("{");
    Position pos = token->pos;
    pos.end_col = (token->type == NEWLINE) ? 0 : pos.end_col;

    print_position(pos);
    const char* title = cons_token_type_colored(token->type);
    printf("%s: '", title);
    free((void*)title);

    print_token_value(token);
    printf("'}\n");
}
