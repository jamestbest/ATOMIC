//
// Created by james on 25/12/23.
//

#include "Lexer.h"

// [[todo]] nav is not a type it is a value like `true` or `false` it is a null ptr

static uint lex_line(const Buffer *line);

static PosCharp longest_word_in_arr(const char* word, Arr arr);
static ArrPosCharp longest_word_in_arrs(const char* word, uint arr_count, ...);
static PosCharp word_in_arr(const char* word, Arr arr);
static void lex_word(void);
static void lex_identifier(void);
static uint lex_number(void);
static encodedType lex_type_to_encoded_int(ATOM_CT__LEX_TYPES_ENUM enum_position);
static int create_multiline_value(const char* starter, const char* delimiter, TokenType type, bool include_delimiter);
static int lex_string_lit(void);
static int lex_character_lit(void);
static uint lex_operator(PosCharp operator);
static int lex_comment(void);
static int lex_multiline_comment(void);

static Token create_token(TokenType type, const void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col);
static Token create_multiline_token(TokenType type, void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col, uint32_t start_line);
static Token construct_multiline_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col, uint32_t start_line);
static Token construct_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col);
static Token create_simple_token(TokenType type, uint32_t start_col, uint32_t end_col);
static void add_token(Token t);

static uint32_t current_char(void);
static uint32_t peer(int amount);
static uint32_t peek(void);
static void update_line_count(void);

static char* consume(void);
static char* gourge_unsafe(uint32_t bytes, uint32_t new_col);
static char* gourge(int amount);
static uint32_t get_utf_char_bytes(const char* character);
static uint32_t consume_utf_char(char *start, bool consume, char** next_char);

static void highlight_current_line_err(Position pos);
static char* lexerr_process_char(char a, char buff[2]);
static void lexwarn(Lexwarns warnCode, Position pos, ...);
static uint lexerr(Lexerrors errorCode, Position pos, ...);

uint line_num, col_num;
char* c_char;

Token_vec base_tokens;
FILE* c_file;
Buffer line_buffer;

bool load_line(void) {
    update_line_count();
    c_char = line_buffer.data;
    return get_line(c_file, &line_buffer);
}

//new plan for the lexer
//it will print out any errors it finds, makes more sense to use the information held inside
//it will not return an error code as multiple errors could have occurred so just err code 1
uint lex(FILE* file, Token_vec* folded_tokens, Vector *lines) {
    line_num = 0;
    col_num = 1;
    c_char = NULL;

    line_buffer = buffer_create(BUFF_MIN);

    c_file = file;

    uint retCode = SUCCESS;

    base_tokens = Token_vec_create(folded_tokens->size);

    while (load_line()) {
        const uint errcode = lex_line(&line_buffer);

        if (errcode != SUCCESS) {
            retCode = errcode;
        }

        vector_add(lines, buffer_steal(&line_buffer, BUFF_MIN));
    }

    Token_vec_add(&base_tokens, create_simple_token(EOTS, col_num, col_num));

    fold(&base_tokens, folded_tokens);

    Token_vec_destroy(&base_tokens);

    buffer_destroy(&line_buffer);
    return retCode;
}

uint lex_line(const Buffer* line) {
    while (c_char < line->data + line->pos && current_char() != '\0') {
        const uint32_t c = current_char();

        if (current_char() == '\n') {
            add_token((Token){NEWLINE, .data.ptr = NULL, (Position){line_num, col_num, line_num + 1, 0}});
            consume();
            continue;
        }

        TokenType type;
        switch (current_char()) {
            case ')':
                type = PAREN_CLOSE;
                break;
            case '(':
                type = PAREN_OPEN;
                break;
            case ']':
                type = BRACKET_CLOSE;
                break;
            case '[':
                type = BRACKET_OPEN;
                break;
            case '}':
                type = CURLY_CLOSE;
                break;
            case '{':
                type = CURLY_OPEN;
                break;
            case ',':
                type = COMMA;
                break;
            case ';':
                type = DELIMITER;
                break;
            case ':': {
                uint32_t next = peek();
                if (next == ':') {
                    consume();
                    type = TYPE_IMPL_CAST;
                } else {
                    type = TYPE_SET;
                }
                break;
            }
            case ' ':
                type = WS_S;
                break;
            case '\t':
                type = WS_T;
                break;

            default:
                goto lex_line_cont;
        }

        add_token(create_simple_token(type, col_num, col_num));
        consume();
        continue;

    lex_line_cont:
        switch (current_char()) {
            case COMMENT_START_ASCII_CODE:
                lex_comment();
                break;
            case '"':
                lex_string_lit();
                break;
            case '\'':
                lex_character_lit();
                break;
            default:
                goto lex_line_cont_two;
        }
        continue;

    lex_line_cont_two:
        if (is_alph(c)) {
            lex_word();
        }
        else if (is_digit(c)) {
            const uint errcode = lex_number();
            if (errcode != SUCCESS) return errcode;
        }
        else {
            const PosCharp op_pos = longest_word_in_arr(c_char, ATOM_CT__LEX_OPERATORS);
            if (op_pos.arr_pos != -1){
                lex_operator(op_pos);
            } else {
                consume();
            }
        }
    }

    return SUCCESS;
}

void print_tokens(Token_vec* token_vec, bool include_ws, bool include_comments) {
    puts("TOKENS");
    for (uint i = 0; i < token_vec->pos; i++) {
        TokenType type = token_vec->arr[i].type;

        if ((type == WS_S || type == WS_T) && !include_ws) continue;
        if ((type == COMMENT) && !include_comments) continue;

        print_token(&token_vec->arr[i]);
    }
}

void print_verbose_tokens(Token_vec* token_vec, Vector* lines, bool print_labels) {
    if (token_vec == NULL || lines == NULL) {
        printf("ERROR PRINTING TOKENS, found NULL for lines or token storage");
        return;
    }

    printf("KEY:\n"
            C_BLU"IDENTIFIER\n"
            C_RED"KEYWORD\n"
            C_CYN"CONSTANT\n"
            C_MGN"TYPE\n"
            C_WHT"WHITESPACE/COMMENTS\n"
            C_GRN"OPERATORS\n\n");

    //[[todo]] perhaps change to errors
    if (token_vec->pos == 0) {
        printf("NO TOKENS FOUND");
        return;
    }

    uint current_tok_pos = 0;
    Token* current_tok = &token_vec->arr[current_tok_pos];

    for (uint i = 0; i < lines->pos; i++) {
        while (current_tok->pos.start_line == i + 1) { //[[todo]] won't work with multiline base_tokens
            printf("%s", get_token_color(current_tok->type));
            print_token_value(current_tok);
            printf(C_RST);

            if (++current_tok_pos >= token_vec->pos) break;
            current_tok = &token_vec->arr[current_tok_pos];
        }
        putchar('\n');
    }
}

ArrPosCharp longest_word_in_arrs(const char* word, const uint arr_count, ...) {
    va_list vl;

    va_start(vl, arr_count);

    ArrPosCharp ret = (ArrPosCharp){NULL, (PosCharp){-1, NULL}};
    for (uint i = 0; i < arr_count; ++i) {
        const Arr array = va_arg(vl, Arr);

        const PosCharp res = longest_word_in_arr(word, array);

        if (res.arr_pos == -1) continue;

        if (ret.array == NULL || ret.posCharp.arr_pos == -1) {
            ret.posCharp = res;
            ret.array = array.arr;
            continue;
        }

        if (len(ret.array[ret.posCharp.arr_pos]) < len(array.arr[res.arr_pos])) {
            ret.posCharp = res;
            ret.array = array.arr;
        }
    }

    va_end(vl);

    return ret;
}

PosCharp longest_word_in_arr(const char* word, const Arr arr) {
    PosCharp ret = {-1, NULL};

    for (uint i = 0; i < arr.elem_count; ++i) {
        const char* w_arr = arr.arr[i];

        const int final_pos = starts_with(word, w_arr);
        if (final_pos != -1) {
            if (ret.arr_pos != -1 && len(w_arr) < len(arr.arr[ret.arr_pos])) continue;

            ret = (PosCharp){i, (char*)(word + final_pos)};
        }
    }

    return ret;
}

PosCharp word_in_arr(const char *word, Arr arr) {
    for (uint i = 0; i < arr.elem_count; i++) {
        const char* w_arr = arr.arr[i];

        const int final_pos = starts_with_ic(word, w_arr);
        if (final_pos != -1) {
            return (PosCharp) {(int)i, (char*)(word + final_pos)};
        }
    }

    return (PosCharp){-1, NULL};
}

int create_multiline_value(const char* starter, const char* delimiter, const TokenType type, const bool include_delimiter) {
    Buffer ml_comment_buff = buffer_create(BUFF_MIN);

    const uint32_t start_line = line_num;
    const uint32_t start_col = col_num;

    if (!include_delimiter) gourge(len(starter));

    const char* starting_pos = c_char + strlen(starter);

    const char* comment_end = strstr(starting_pos, delimiter);
    if (comment_end == NULL) {
        do {
            buffer_concat(&ml_comment_buff, c_char);

            //A new line needs to be loaded into the line_buffer
            const bool ret = load_line();

            if (!ret) goto create_multiline_value_fail;

            comment_end = strstr(c_char, delimiter);
        } while (comment_end == NULL);
    }
    //the current line contains the comment end

    if (include_delimiter) comment_end += strlen(delimiter);

    const uint32_t d_size = comment_end - c_char;
    buffer_nconcat(&ml_comment_buff, c_char, d_size);

    uint end_col = 0;

    while (c_char < comment_end) {
        end_col++;
        c_char += get_utf_char_bytes(c_char);
    }

    col_num = end_col;

    add_token(construct_multiline_token(type, buffer_steal(&ml_comment_buff, 0), start_col, end_col, start_line));

    if (!include_delimiter) gourge(len(delimiter));

    buffer_destroy(&ml_comment_buff);
    return SUCCESS;

create_multiline_value_fail:
    buffer_destroy(&ml_comment_buff);
    return FAIL;
}

int lex_string_lit(void) {
    return create_multiline_value("\"", "\"", LIT_STR, false);
}

int lex_character_lit(void) {
    /* This could either be a character or just the ' symbol
     * The ' won't be used in ATOM lang for the moment but I'll still lex it here
     */

    uint32_t possible_end = peer(2);

    if (possible_end != '\'') {
        add_token(create_simple_token(CARROT, col_num, col_num));
        consume();
        return SUCCESS;
    }

    consume(); //eat the '
    char* c_start = c_char;
    uint32_t start_col = col_num;

    consume(); //eat the character
    char* c_end = c_char;
    uint32_t end_col = col_num;

    consume(); //eat the '

    add_token(create_token(LIT_CHR, c_start, c_end - c_start, start_col, end_col));
    return SUCCESS;
}

//[[todo]] need to have error checking for multiline comment not ending
//         can only happen if reach EOF
int lex_comment(void) {
    if (peek() == '*') {
        return lex_multiline_comment();
    }

    const uint col_start = col_num;
    char* comment_start = c_char;

    consume(); //eat the ¬

    while (current_char() != '\n') {
        consume();
    }

    const uint bytes = c_char - comment_start;

    add_token(create_token(COMMENT, comment_start, bytes * sizeof(char), col_start, col_num - 1));

    return SUCCESS;
}

int lex_multiline_comment(void) {
    return create_multiline_value("¬*", "*¬", COMMENT, true);
}

uint lex_operator(const PosCharp operator) {
    const uint32_t op_size = strlen(ATOM_CT__LEX_OPERATORS.arr[operator.arr_pos]);

    add_token(create_token(operator_to_type(operator.arr_pos), (void*)&(operator.arr_pos), sizeof(int), col_num, col_num + op_size));

    gourge(op_size);
    return SUCCESS;
}

uint lex_number(void) {
    /*
     * Could be a LIT_FLOAT, or a LIT_INT
     * [1-9][0-9]*(.[0-9]*)?
     * INT literals can have base prefixes
     */

    uint start_col = col_num;

    int base = 10;
    if (current_char() == '0' && is_alph(peek())) {
        consume(); //remove the 0 as its not the value
        switch(current_char()) { //a starting 0 wouldn't affect the value anyway
            case 'b':
            case 'B':
                base = 2;
                break;
            case 't':
            case 'T':
                base = 3;
                break;
            case 'o':
            case 'O':
                base = 8;
                break;
            case 'd':
            case 'D':
                base = 10;
                break;
            case 'x':
            case 'X':
            case 'h': // maybe
            case 'H':
                base = 16;
                break;
            default:
                lexerr(LEXERR_INT_INVALID_BASE, (Position){line_num, col_num, line_num, col_num},
                       c_char);
                return FAIL;
        }
        consume(); //eat the base
    }

    start_col = start_col + 0; //silence compiler

    const char* start_char = c_char;
    //we know that the first character is valid
    char* end_char = consume();

    while (is_digit_base(current_char(), base)) {
        end_char = consume();
    }

    if (current_char() == '.') {
        //FLOAT
        const char* decimal_pos = c_char;
        consume();

        while (is_digit_base(current_char(), base)) {
            end_char = consume();
        }

        if (end_char == decimal_pos) {
            lexerr(LEXERR_FLOAT_TRAILING_DECIMAL, (Position){line_num, start_col, line_num, col_num - 1});
            return FAIL;
        }

        char* value_end;
        long double value = strtold(start_char, &value_end);

        if (value_end != end_char) {
            lexerr(LEXERR_FLOAT_INVALID_FLOAT, (Position){line_num, start_col, line_num, col_num - 1}, end_char - 1, value_end - 1);
            return FAIL;
        }

        add_token(construct_token(LIT_FLOAT, &value, start_col, col_num - 1));

        return SUCCESS;
    } else {
        //INTEGER
        char* value_end;
        llint value = strtoll(start_char, &value_end, base);

        if (value_end != end_char) {
            lexerr(LEXERR_INT_INVALID_INT, (Position){line_num, start_col, line_num, col_num - 1}, end_char - 1, value_end - 1);
            return FAIL;
        }

        const uint end_col = col_num;

        const char* error_end_char = end_char;
        while (is_alph_numeric(current_char())) {
            error_end_char = consume();
        }

        if (value_end != error_end_char) {
            lexerr(LEXERR_INT_INVALID_DIGIT_FOR_BASE,
                   (Position){line_num, end_col, line_num, col_num - 1},
                   base, end_char);
            return FAIL;
        }

        add_token(construct_token(LIT_INT, &value, start_col, col_num - 1));

        return SUCCESS;
    }
}

void lex_word(void) {
    /* A word could be:
     *  - KEYWORD
     *  - TYPE
     *  - OPERATOR e.g. and, or, xor, not
     *  - IDENTIFIER (VAR NAME, FUNCTION NAME)
     *  - CONSTANT IDENTIFIERS (true, false)
     */

    PosCharp info;

    if (info = word_in_arr(c_char, ATOM_CT__LEX_KEYWORDS), info.arr_pos != -1) {
        const uint64_t char_count = info.next_char - c_char;

        add_token(create_token(KEYWORD, &info.arr_pos, -1, col_num, col_num + char_count - 1));

        gourge(info.next_char - c_char);
        return;
    }

    if (info = word_in_arr(c_char, ATOM_CT__LEX_TYPES), info.arr_pos != -1) {
        const uint64_t char_count = info.next_char - c_char;

        encodedType encoded_type = lex_type_to_encoded_int(info.arr_pos);

        add_token(create_token(TYPE, &encoded_type, -1, col_num, col_num + char_count - 1));

        gourge(info.next_char - c_char);
        return;
    }

    if (info = word_in_arr(c_char, ATOM_CT__LEX_CONS_IDENTIFIERS), info.arr_pos != -1) {
        const uint64_t char_count =  strlen(ATOM_CT__LEX_CONS_IDENTIFIERS.arr[info.arr_pos]);
        add_token(create_token(LIT_BOOL, ATOM_CT__LEX_CONS_IDENTIFIERS.arr[info.arr_pos],
                               (char_count + 1) * sizeof(char), col_num, col_num + char_count - 1));
        gourge(info.next_char - c_char);
        return;
    }

    if (info = word_in_arr(c_char, ATOM_CT__LEX_OP_IDENTIFIERS), info.arr_pos != -1) {
        Token token;
        token.type = OP_BIN;
        token.pos.start_line = line_num;
        token.pos.end_line = line_num;
        token.pos.end_col = col_num;

        switch (info.arr_pos) {
            case AND:
                token.data.enum_pos = LAND;
                break;
            case OR:
                token.data.enum_pos = LOR;
                break;
            case XOR:
                token.data.enum_pos = LXOR;
                break;
            case NOT:
                token.data.enum_pos = LNOT;
                token.type = OP_UN_PRE;
                break;
            case AS:
//                token.data

            default:
                assert(false);
        }

        token.pos.start_col = col_num;
        token.pos.end_col = col_num + len(ATOM_CT__LEX_OP_IDENTIFIERS.arr[info.arr_pos]);

        add_token(token);
        gourge(info.next_char - c_char);
        return;
    }

    lex_identifier();
}

bool is_valid_identifier_end(uint32_t chr) {
    if (is_alph_numeric(chr)) return true;

    if (chr == '_') return true;

    return false;
}

void lex_identifier(void) {
    const uint32_t start_col = col_num;

    char* start = c_char;
    consume(); //we know that the first character is alph() as that's why we're here
    const char* end = c_char;

    while (is_valid_identifier_end(current_char())) {
        end = consume();
    }

    const uint32_t num_chars = (end - start);
    add_token(create_token(IDENTIFIER, start, num_chars * sizeof(char), start_col, start_col + num_chars - 1));
}

encodedType lex_type_to_encoded_int(ATOM_CT__LEX_TYPES_ENUM enum_position) {
    ATOM_CT__LEX_TYPES_GENERAL_ENUM general_type = -1;
    uint64_t size = 0;
    uint64_t ptr_offset = 0;

    if (enum_position >= ATOM_CT__LEX_TYPES.elem_count != 0) {
        return (encodedType){-1, -1, -1, -1};
    }

    switch (enum_position) {
        case I1:
        case I2:
        case I4:
        case I8:
            general_type = INTEGER;
            break;

        case N1:
        case N2:
        case N4:
        case N8:
            general_type = NATURAL;
            break;

        case R4:
        case R8:
        case R10:
            general_type = REAL;
            break;

        case Q4:
        case Q8:
        case Q16:
            general_type = RATIONAL;
            break;

        case STR:
            general_type = STRING;
            goto lex_type_to_encoded_int_ptr;

        case CHR:
            general_type = CHAR;
            goto lex_type_to_encoded_int_ptr;

        case BOOL:
            general_type = BOOLEAN;
            goto lex_type_to_encoded_int_ptr;

        case NAV:
            general_type = NOT_A_VALUE;
            goto lex_type_to_encoded_int_ptr;
    }

    char* type = ATOM_CT__LEX_TYPES.arr[enum_position];

    if (strlen(type) < 2) {
        return (encodedType){-1, -1, -1, -1};
        //todo error
    }

    // e.g. i4, r8, n2, q16
    char* end;
    llint type_size = strtoll(type + 1, &end, 10);
    if (end != type + strlen(type) - 1) {
        //todo error
    }

    size = type_size;

lex_type_to_encoded_int_ptr:
    return (encodedType) {
        general_type,
        size,
        ptr_offset,
        enum_position
    };
}

Token create_simple_token(const TokenType type, const uint32_t start_col, const uint32_t end_col) {
    Token t;

    t.type = type;
    t.pos = (Position){line_num, start_col, line_num, end_col};
    t.data.ptr = NULL;

    return t;
}

Token construct_multiline_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col, uint32_t start_line) {
    Token ret = create_simple_token(type, start_col, end_col);
    ret.data.ptr = made_data; //assume its a ptr value for multiline [[todo]] affirm
    ret.pos.start_line = start_line;

    return ret;
}

static Token construct_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col) {
    return create_token(type, made_data, -1, start_col, end_col);
}

/* Token type is self-explanatory, the list is in the Tokens.h file
 * Data is a pointer to anything that may need to be stored e.g. for identifiers or keywords its a char* to
 * the word, for a number its a pointer to a 64 bit number, for a float a pointer to a double, ect
 * The data capacity is just for the malloc
 * start and end col e.g. 123, <1:3>
 */
Token create_token(TokenType type, const void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col) {
    Token t;

    t.type = type;
    t.pos = (Position){line_num, start_col, line_num, end_col};
    t.data.ptr = NULL;

    if (data == NULL) {
        return t;
    }

    switch (type) {
        case IDENTIFIER:
        case LIT_BOOL:
        case LIT_STR:
        case LIT_CHR:
        case COMMENT:
            t.data.ptr = malloc(d_size + 1);
            memcpy(t.data.ptr, data, d_size);
            ((char*)t.data.ptr)[d_size] = '\0';
            break;
        case LIT_INT:
            t.data.integer = *(llint*)data;
            break;
        case LIT_FLOAT:
            t.data.real = *(long double*)data;
            break;
        case OP_BIN:
        case OP_UN:
        case OP_TRINARY:
        case OP_BIN_OR_UN:
        case ARITH_ASSIGN:
        case KEYWORD:
            t.data.enum_pos = *(int *)data;
            break;
        case TYPE:
            t.data.type = *(encodedType *)data;
            break;
        case ASSIGN:
        case BRACKET_OPEN:
        case BRACKET_CLOSE:
        case CURLY_OPEN:
        case CURLY_CLOSE:
        case PAREN_OPEN:
        case PAREN_CLOSE:
        case COMMA:
        case CARROT:
        case TYPE_SET:
        case TYPE_IMPL_CAST:
        case DELIMITER:
        case LIT_NAV:
        case WS_S:
        case WS_T:
        case NEWLINE:
        case EOTS:
        case TOKEN_INVALID:
            break;
    }

    return t;
}

void add_token(Token t) {
    Token_vec_add(&base_tokens, t);
}

uint32_t current_char(void) {
    return consume_utf_char(c_char, false, NULL);
}

uint32_t peer(int amount) {
    char* current_start = c_char;
    char* current_end; //could prob be the same

    uint32_t ret = 0;

    while (amount >= 0) {
        ret = consume_utf_char(current_start, false, &current_end);
        current_start = current_end;
        amount--;
    }

    return ret;
}

uint32_t peek(void) {
    return peer(1);
}

char* gourge_unsafe(const uint32_t bytes, const uint32_t new_col) {
    c_char += bytes;
    col_num = new_col;

    return c_char;
}

char* gourge(int amount) {
    for (int i = 0; i < amount; i++) consume();

    return c_char;
}

void update_line_count(void) {
    line_num++;
    col_num = 1;
}

char* consume(void) {
    consume_utf_char(c_char, true, NULL);

    col_num++;

    return c_char;
}

uint32_t get_utf_char_bytes(const char* character) {
    const uchar fbyte = *character;
    uint byte_count = 1;

    if ((fbyte & 0xE0) == 0xC0) {
        byte_count = 2;
    }
    else if ((fbyte & 0xf0) == 0xe0) {
        byte_count = 3;
    }
    else if ((fbyte & 0xf8) == 0xf0 && (fbyte <= 0xf4)) {
        byte_count = 4;
    }
    else {
        byte_count = 1;
    }

    return byte_count;
}

uint32_t consume_utf_char(char *start, bool consume, char** next_char) {
    const uchar fbyte = *start;
    uint32_t ret = fbyte;

    uint inc = 1;

    if (fbyte < 0x80) {
        //1 byte character
    }
    else if ((fbyte & 0xE0) == 0xC0) {
        ret = ((fbyte & 0x1F) << 6)             |
                (start[1] & 0x3f);
        inc = 2;
    }
    else if ((fbyte & 0xf0) == 0xe0) {
        ret = ((fbyte & 0x0f) << 12)            |
                ((start[1] & 0x3f) << 6)        |
                (start[2] & 0x3f);
        inc = 3;
    }
    else if ((fbyte & 0xf8) == 0xf0 && (fbyte <= 0xf4)) {
        ret = ((fbyte & 0x07) << 18)            |
                ((start[1] & 0x3f) << 12)       |
                ((start[2] & 0x3f) << 6)        |
                ((start[3] & 0x3f));
        inc = 4;
    }
    else {
        ret = -1; //allow it to skip the byte if consume();
    }

    if (consume) c_char += inc;
    if (next_char) *next_char = start + inc;
    return ret;
}

char* lexerr_process_char(char a, char buff[2]) {
    if (a == '\n') return "\\n";
    if (a == '\t') return "\\t";

    buff[0] = a;
    return buff;
}

uint lexerr(const Lexerrors errorCode, const Position pos, ...) {
    va_list args;

    va_start(args, pos);

    char buff1[2] = {'f', '\0'};
    char buff2[2] = {'f', '\0'};

    printf(C_RED"LEXERR: "C_RST);

    bool print_current_line = true;

    //the line we are on is still in its buffer
    switch (errorCode) {
        case LEXERR_INT_INVALID_INT: {
            char* atom_end = lexerr_process_char(*va_arg(args, char*), buff1);
            char* strtoll_end = lexerr_process_char(*va_arg(args, char*), buff2);
                    printf(ATOM_CT__LEXERR_INT_INVALID_INT,
                           atom_end,
                           strtoll_end);
            break;
        }
        case LEXERR_INT_INVALID_BASE: {
            char* end_char = lexerr_process_char(*va_arg(args, char*), buff1);
            printf(ATOM_CT__LEXERR_INT_INVALID_BASE,
                   end_char);
            break;
        }
        case LEXERR_INT_INVALID_DIGIT_FOR_BASE: {
            int base = va_arg(args, int);
            char* start_char = lexerr_process_char(*va_arg(args, char*), buff1);
            printf(ATOM_CT__LEXERR_INT_INVALID_DIGIT_FOR_BASE,
                   base,
                   start_char);
            break;
        }
        case LEXERR_FLOAT_TRAILING_DECIMAL: {
            printf(ATOM_CT__LEXERR_FLOAT_TRAILING_DECIMAL);
            break;
        }
        case LEXERR_FLOAT_INVALID_FLOAT: {
            char* atom_end = lexerr_process_char(*va_arg(args, char*), buff1);
            char* strtold_end = lexerr_process_char(*va_arg(args, char*), buff2);
            printf(ATOM_CT__LEXERR_FLOAT_INVALID_FLOAT,
                   atom_end,
                   strtold_end);
            break;
        }
        case LEXERR_COMMENT_MULTILINE_NO_END: {
            printf(ATOM_CT__LEXERR_COMMENT_MULTILINE_NO_END);
            print_current_line = false;
            break;
        }
    }

    if (print_current_line) highlight_current_line_err(pos);
    else {
        print_position(pos);
        printf("\n");
    }

    va_end(args);

    return errorCode;
}

void lexwarn(Lexwarns warnCode, Position pos, ...) {
    va_list args;

    va_start(args, pos);

    //the line we are on is still in its buffer
    switch (warnCode) {
        case LEXWARN_INT_MISSING_BASE:
            break;
    }
    highlight_current_line_err(pos);
}

void highlight_current_line_err(Position pos) {
    highlight_line_err(pos, line_buffer.data);
}
