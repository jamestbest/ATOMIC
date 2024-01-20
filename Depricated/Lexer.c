//
// Created by james on 28/10/23.
//

#include "Lexer.h"

uint warnCount, errCount;

uint line_num, col_num;

char* c_char;

LexRet lex(FILE *file, Token_vec *token_vec, Vector *lines) {
    /*
     * Need to read in the file and turn the strings into tokens
     */

    warnCount = errCount = 0;
    line_num = col_num = 1;

    /* Streaming tokens? not at the moment just store all the lines in a buffer and then store information on the position and size of a token */
    Buffer buffer = buffer_create(BUFF_SIZE);

    while (get_line(file, &buffer)) {
        line_to_tokens(&buffer, token_vec);

        // update: removed stealing the line as the data is now copied into the tokens
//        line_num++; removed to finding newline tokens instead in the consume function
    }

    buffer_destroy(&buffer);

    print_tokens(token_vec);

    return (LexRet) {errCount, warnCount};
}

char* gourge(uint amount) {
    for (uint i = 0; i < amount; i++) consume();
    //could probably have an unsafe variant that doesn't check for newlines
    //if we can assume that we're jumping forward into something like an identifier
    //for now ill just keep it as a looped consume

    return c_char;
}

//c_char aptly points to the c_char character being processed
char* consume(void) {
    col_num++;

    c_char++;

    if (is_newline(*c_char)) {
        line_num++;
        col_num = 1;
    }
    return c_char;
}

char* peek(void) {
    return c_char + 1;
}

/*
 * type of token
 * a pointer to the data to be stored, can be heap or stack allocated, as long as valid
 * the size of the data in bytes
 */
Token create_token(TokenType type, void* data, size_t d_size, uint line, uint start_col, uint end_col) {
    void* data_loc = NULL;

    if (d_size != 0 && data != NULL) {
        data_loc = malloc(d_size);
        memcpy(data_loc, data, d_size);
    }

    Token t = (Token){type, data_loc, line, start_col, end_col};

    return t;
}

void add_token(TokenType type, void* data, size_t d_size, uint d_length, Token_vec* token_vector) {
    Token t = create_token(type, data, d_size, line_num, col_num, col_num + d_length);

    Token_vec_add(token_vector, t);
}

void print_tokens(Token_vec* tokens) {
    for (uint i = 0; i < tokens->pos; i++) {
        print_token(&tokens->arr[i]);
    }
}

void print_token(Token* t) {
    //{<position> TYPE : Value}
    printf("{");

    printPosition(t->line, t->start_col, t->type == NEWLINE ? t->line + 1 : t->line, t->end_col);

    switch (t->type) {
        case LIT_INT:
            printf("INT_LIT: %ld", *(int64_t*)t->data);
            break;
        case IDENTIFIER:
            printf("IDENTIFIER: ");
            goto printword;
        case KEYWORD:
            printf("KEYWORD: ");
            goto printword;
        printword:
            printf("%s", (char*)t->data);
            break;

        default:
            printf("NOT HANDLED :: VALUE");
            break;
    }

    printf("}\n");
}

/*
 * convert a char* to vec of tokens
 *
 * Words - Identifiers, Keywords,
 * Numbers - Floats/Ints
 * Strings/chars
 * Operators
 */
void line_to_tokens(Buffer* line, Token_vec* tokens) {
    c_char = line->data;

    while (*consume() != '\0') {
        switch (*c_char) {
            case '[':
                add_token(BRACKET_OPEN, NULL, 0, 1, tokens);
                break;
            case ']':
                add_token(BRACKET_CLOSE, NULL, 0, 1, tokens);
                break;
            case '(':
                add_token(PAREN_OPEN, NULL, 0, 1, tokens);
                break;
            case ')':
                add_token(PAREN_CLOSE, NULL, 0, 1, tokens);
                break;
            case '{':
                add_token(CURLY_OPEN, NULL, 0, 1, tokens);
                break;
            case '}':
                add_token(CURLY_CLOSE, NULL, 0, 1, tokens);
                break;
        }

        if (is_digit(*c_char)) {
            //has to be a number
            Token t = lex_num();
            Token_vec_add(tokens, t);
        }
        else if (is_alph(*c_char)) {
            Token t = lex_word();
            Token_vec_add(tokens, t);
        }
        else {
//            lex_operator();
        }
    }
}

/*
 * is start found in the array
 * start is just the start of a string
 */
PosCharp is_in(const char* start, Arr arr) {
    for (int i = 0; i < (int) arr.size; i++) {
        int pos;
        if (pos = starts_with_ic(start, arr.arr[i]), pos != -1) {
            return (PosCharp){i, (char*)start + pos};
        }
    }
    return (PosCharp){-1, NULL};
}

char* create_heap_copy(const char* const_data) {
    uint length = len(const_data) + 1;

    char* data = malloc(length * sizeof(char));

    memcpy(data, const_data, length);

    return data;
}

Token lex_op_identifier(PosCharp position) {
    Token ret = (Token){OP_BIN, NULL, line_num, col_num, -1};

    switch (position.pos) {
        case 0: //and
            ret.data = create_heap_copy("&&");
            return ret;
        case 1: //or
            ret.data = create_heap_copy("||");
            return ret;
        case 2: //xor
            ret.data = create_heap_copy("|-|");
            return ret;
        case 3: //not
            ret.data = create_heap_copy("!");
            ret.type = OP_UN_PRE;
            return ret;
        default:
            assert(false);
    }
}

Token lex_word(void) {
    Token ret;

    ret.line = line_num;
    ret.start_col = col_num;
    ret.end_col = -1;

    /* An identifier could be a:
     *   - KEYWORD
     *   - TYPE
     *   - OPERATOR
     *   - IDENTIFIER (variable name, function name)
     */

    //if is_in ect ect blah blah blah
    PosCharp comp = is_in(c_char, ATOM_CT__LEX_KEYWORDS);
    if (comp.pos != -1) {
        ret.type = KEYWORD;
        ret.data = NULL;
        ret.end_col = col_num + (comp.last - c_char);

        return ret;
    }

    comp = is_in(c_char, ATOM_CT__LEX_OP_IDENTIFIERS);
    if (comp.pos != -1) {
        return lex_op_identifier(comp);
    }

    comp = is_in(c_char, ATOM_CT__LEX_TYPES);
    if (comp.pos != -1) {
        ret.type = TYPE;

        uint size = comp.last - c_char;
        char* data = malloc( size* sizeof(char) + 1);
        memcpy(data, c_char, size);
        data[size] = '\0';

        ret.data = data;
        ret.end_col = col_num;
    }

    ret.type = IDENTIFIER;

    char* start = c_char;
    uint size = 1; //one character has already been checked and is alph()
    while (is_alph_numeric(*(peek()))) {
        size++;
        consume();
    }

    char* data = malloc(size * sizeof(char) + 1);

    memcpy(data, start, size * sizeof(char));

    data[size] = '\0';

    ret.data = data;
    ret.end_col = col_num;

    return ret;
}

Token lex_num(void) {
    //check if it is a base prefixed number
    /* Supported bases
     *  0b - binary
     *  0x - hex
     *  0o - oct
     */

    /* Can be either an LIT_INT, or a LIT_FLOAT
     *
     * NUM :: [0-9]
     *
     */

    Token t = (Token) {LIT_INT, NULL, line_num, col_num, -1};

    /*
     * The valid digits depends on the base
     * If an invalid digit is found then just check if it is a base 10 digit, if it is a base 10 digit
     * then assume that it was an incorrect digit error e.g. using 2 in a binary prefixed num e.g. 0b001102
     * else assume that it is the end of the number
     */

    char* next_char;

    int base = 10;

    //can assume the first character is valid from check before
    if (*c_char == '0') {
        //this is the start of the base prefix

        char next = *(consume());

        switch (next) {
            case 'x':
            case 'X':
                base = 16;
                break;
            case 'b':
            case 'B':
                base = 2;
                break;
            case 'd':
            case 'D':
                base = 10;
                break;
            case 'o':
            case 'O':
                base = 8;
                break;
            case 't':
            case 'T':
                base = 3;
                break;
            default:
                if (is_digit(next)) {
                    lexWarn(line_num, col_num, line_num, col_num, ATOM_CT__LEX_WRN_INT_LIT_BASE_MISSING);
                }
                else if (!is_whitespace(*c_char) && !is_newline(*c_char)){
                    lexError(line_num, col_num, line_num, col_num, ATOM_CT__LEX_ERR_INT_LIT_BASE_MISMATCH, *c_char);
                }
                break;
        }

        consume(); //skip over the base specifier
    }

    //strtoll supports prefixes for hex (0x/X), and octal (0?). Not sure if that's the correct
    //octal prefix but I don't like it so I'll use hex but binary will be seperate check
    int64_t value = strtoll(c_char, &next_char, base);

    void* data = malloc(sizeof(int64_t));

    memcpy(data, &value, sizeof(int64_t));

    t.data = data;
    t.end_col = next_char - c_char;

    printf("DEBUG: FOUND INT LITERAL WITH VALUE %ld\n", value);

    c_char = next_char - 1; //[[todo]]: check is this fine if invalid num at start will it go back too far

    return t;
}

void printPosition(uint startLine, uint startCol, uint endLine, uint endCol) {
    printf("<%d:%d", startLine, startCol);

    if (endLine == (uint)-1 || startLine == endLine) {
        if (startCol != endCol) printf("-%d", endCol);
    } else {
        printf("-%d:%d", endLine, endCol);
    }

    printf("> ");
}

void lexError(uint startLine, uint startCol, uint endLine, uint endCol, const char* error, ...) {
    printPosition(startLine, startCol, endLine, endCol);

    va_list args;

    va_start(args, error);

    fputs(ERROR_BASE, stdout);
    vprintf(error, args);

    errCount++;
}

void lexWarn(uint startLine, uint startCol, uint endLine, uint endCol, const char* error, ...) {
    printPosition(startLine, startCol, endLine, endCol);

    va_list args;

    va_start(args, error);

    fputs(WARN_BASE, stdout);
    vprintf(error, args);

    warnCount++;
}
