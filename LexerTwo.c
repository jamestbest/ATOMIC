//
// Created by james on 25/12/23.
//

#include "LexerTwo.h"

uint line_num, col_num;
char* c_char;

Token_vec* tokens;
FILE* c_file;
Buffer line_buffer;

bool in_multiline_comment;
Buffer multiline_buffer;

static uint32_t global_start_line;
static uint32_t global_start_col; //ik ik

//new plan for the lexer
//it will print out any errors it finds, makes more sense to use the information held inside
//it will not return an error code as multiple errors could have occurred so just err code 1
uint lex(FILE* file, Token_vec* token_vec, Vector *lines) {
    line_num = 1;
    col_num = 0;
    c_char = NULL;

    tokens = token_vec;

    line_buffer = buffer_create(BUFF_MIN);

    multiline_buffer = buffer_create(BUFF_MIN);
    in_multiline_comment = false;

    c_file = file;

    uint retCode = SUCCESS;

    while (get_line(file, &line_buffer)) {
        c_char = line_buffer.data;

        uint errcode = lex_line(&line_buffer);

        if (errcode != SUCCESS) {
            retCode = errcode;
        }

        vector_add(lines, buffer_steal(&line_buffer, BUFF_MIN));
    }

    if (in_multiline_comment) {
        lexerr(LEXERR_COMMENT_MULTILINE_NO_END, (Position){global_start_line, global_start_col, global_start_line, global_start_col});
        retCode = LEXERR_COMMENT_MULTILINE_NO_END;
    }

    Token_vec_add(tokens, create_simple_token(EOTS, col_num, col_num));

    buffer_destroy(&line_buffer);
    return retCode;
}

uint lex_line(Buffer* line) {
    if (in_multiline_comment) {
        lex_multiline_comment();
        return SUCCESS;
    }

    while (c_char < line->data + line->pos && current_char() != '\0') {
        uint32_t c = current_char();

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
            case ':':
                type = TYPE_SET;
                break;
            case ' ':
                type = WS_S;
                break;
            case '\t':
                type = WS_T;
                break;
            case '\n':
                type = NEWLINE;
                break;

            default:
                goto lex_line_cont;
        }

        add_token(create_simple_token(type, col_num, col_num));
        consume();
        continue;

    lex_line_cont:
        switch (consume_utf_char(c_char, false, NULL)) {
            case COMMENT_START_ASCII_CODE:
                lex_comment();
                break;
            default:
                goto lex_line_cont_two;
        }
        return SUCCESS;

    lex_line_cont_two:
        if (is_alph(c)) {
            lex_word();
        }
        else if (is_digit(c)) {
            uint errcode = lex_number();
            if (errcode != SUCCESS) return errcode;
        }
        else {
//            lex_operator();
            consume();
        }

    }

    return SUCCESS;
}

void print_tokens(Token_vec* token_vec) {
    puts("TOKENS");
    for (uint i = 0; i < token_vec->pos; i++) {
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
            C_GRN"OPERATORS\n");

    //[[todo]] perhaps change to errors
    if (token_vec->pos == 0) {
        printf("NO TOKENS FOUND");
        return;
    }

    uint current_tok_pos = 0;
    Token* current_tok = &token_vec->arr[current_tok_pos];

    for (uint i = 0; i < lines->pos; i++) {
        while (current_tok->pos.start_line == i + 1) { //[[todo]] won't work with multiline tokens
            printf("%s", get_token_color(current_tok->type));
            print_token_value(current_tok);
            printf(C_RST);

            if (++current_tok_pos >= token_vec->pos) break;
            current_tok = &token_vec->arr[current_tok_pos];
        }
        putchar('\n');
    }
}

char* create_heap_copy_of_static_string(const char* string){
    size_t length = len(string) + 1;
    char* data = malloc(length * sizeof(char));
    memcpy(data, string, length);

    data[length - 1] = '\0';

    return data;
}

PosCharp word_in_arr(const char *word, Arr arr) {
    for (uint i = 0; i < arr.size; i++) {
        char* w_arr = arr.arr[i];

        int final_pos = starts_with_ic(word, w_arr);
        if (final_pos != -1) {
            return (PosCharp) {(int)i, (char*)(word + final_pos)};
        }
    }

    return (PosCharp){-1, NULL};
}

int lex_comment(void) {
    if (peek() == '*') {
        return lex_multiline_comment();
    }

    uint col_start = col_num;
    char* comment_start = c_char;

    consume(); //eat the ¬

    while (current_char() != '\n') consume();

    uint bytes = c_char - comment_start;

    add_token(create_token_and_term(COMMENT, comment_start, bytes * sizeof(char), col_start, col_num - 1));

    return SUCCESS;
}

int lex_multiline_comment(void) {
    if (!in_multiline_comment) {
        global_start_line = line_num;
        global_start_col = col_num;
        buffer_resize(&multiline_buffer, BUFF_MIN);
        buffer_clear(&multiline_buffer);

        in_multiline_comment = true;
    }

    char* comment_end = strstr(c_char, "*¬");
    if (comment_end == NULL) {
        buffer_concat(&multiline_buffer, c_char);
        update_line_count();
    }
    else {
        in_multiline_comment = false;
        uint32_t d_size = comment_end - c_char + sizeof("*¬") - 1;
        buffer_nconcat(&multiline_buffer, c_char, d_size);

        *(comment_end) = '\0'; //[[todo]] fix
        gourge(strlen(line_buffer.data) + 2);
        *(comment_end) = '*';

        add_token(construct_multiline_token(COMMENT, buffer_steal(&multiline_buffer, 0), global_start_col, col_num, global_start_line));
    }

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

    char* start_char = c_char;
    //we know that the first character is valid
    char* end_char = consume();

    while (is_digit_base(current_char(), base)) {
        end_char = consume();
    }

    if (current_char() == '.') {
        char* decimal_pos = c_char;
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

        long double* data = malloc(sizeof(long double));
        *data = value;

        add_token(construct_token(LIT_FLOAT, data, start_col, col_num - 1));

        return SUCCESS;
    } else {
        char* value_end;
        llint value = strtoll(start_char, &value_end, base);

        if (value_end != end_char) {
            lexerr(LEXERR_INT_INVALID_INT, (Position){line_num, start_col, line_num, col_num - 1}, end_char - 1, value_end - 1);
            return FAIL;
        }

        uint end_col = col_num;

        char* error_end_char = end_char;
        while (is_alph_numeric(current_char())) {
            error_end_char = consume();
        }

        if (value_end != error_end_char) {
            lexerr(LEXERR_INT_INVALID_DIGIT_FOR_BASE,
                   (Position){line_num, end_col, line_num, col_num - 1},
                   base, end_char);
            return FAIL;
        }

        llint* data = malloc(sizeof(llint));
        *data = value;

        add_token(construct_token(LIT_INT, data, start_col, col_num - 1));

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
    TokenType type;

    if (info = word_in_arr(c_char, ATOM_CT__LEX_KEYWORDS), info.arr_pos != -1) {
        type = KEYWORD;
    }
    else if (info = word_in_arr(c_char, ATOM_CT__LEX_TYPES), info.arr_pos != -1) {
        type = TYPE;
    }

    if (info.arr_pos != -1) {
        uint64_t char_count = info.last_char - c_char;
        uint64_t d_byte = (char_count + 1) * sizeof(char);
        add_token(create_token(type, c_char, d_byte, col_num, col_num + char_count - 1));

        gourge(info.last_char - c_char);
        return;
    }

    if (info = word_in_arr(c_char, ATOM_CT__LEX_CONS_IDENTIFIERS), info.arr_pos != -1) {
        uint64_t char_count =  strlen(ATOM_CT__LEX_CONS_IDENTIFIERS.arr[info.arr_pos]);
        add_token(create_token(LIT_BOOL, ATOM_CT__LEX_CONS_IDENTIFIERS.arr[info.arr_pos],
                               (char_count + 1) * sizeof(char), col_num, col_num + char_count - 1));
        gourge(info.last_char - c_char);
        return;
    }

    if (info = word_in_arr(c_char, ATOM_CT__LEX_OP_IDENTIFIERS), info.arr_pos != -1) {
        Token token;
        token.type = OP_BIN;
        token.pos.start_line = line_num;
        token.pos.end_line = line_num;
        token.pos.end_col = col_num;

        switch (info.arr_pos) {
            case 0:
                token.data = create_heap_copy_of_static_string("&&");
                break;
            case 1:
                token.data = create_heap_copy_of_static_string("||");
                break;
            case 2:
                token.data = create_heap_copy_of_static_string("|-|");
                break;
            case 3:
                token.data = create_heap_copy_of_static_string("!");
                token.type = OP_UN_PRE;
                break;
            default:
                assert(false);
        }

        token.pos.start_col = col_num;
        token.pos.end_col = col_num + len(token.data);

        add_token(token);
        gourge(info.last_char - c_char);
        return;
    }

    lex_identifier();
}

void lex_identifier(void) {
    uint32_t start_col = col_num;

    char* start = c_char;
    consume(); //we know that the first character is alph() as that's why we're here
    char* end = c_char;

    while (is_alph_numeric(current_char())) {
        end = consume();
    }

    uint32_t num_chars = (end - start);
    add_token(create_token(IDENTIFIER, start, (num_chars + 1) * sizeof(char), start_col, start_col + num_chars - 1));
}

Token create_simple_token(TokenType type, uint32_t start_col, uint32_t end_col) {
    Token t;

    t.type = type;
    t.pos = (Position){line_num, start_col, line_num, end_col};
    t.data = NULL;

    return t;
}

Token construct_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col) {
    Token ret = create_simple_token(type, start_col, end_col);
    ret.data = made_data;

    return ret;
}

Token create_token_and_term(TokenType type, void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col) {
    Token t = create_token(type, data, d_size + 1, start_col, end_col);

    if (t.data == NULL) return t;

    ((char*)(t.data))[d_size] = '\0';

    return t;
}

Token construct_multiline_token(TokenType type, void* made_data, uint32_t start_col, uint32_t end_col, uint32_t start_line) {
    Token ret = create_simple_token(type, start_col, end_col);
    ret.data = made_data;
    ret.pos.start_line = start_line;

    return ret;
}

Token create_multiline_token(TokenType type, void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col, uint32_t start_line) {
    Token ret = create_token(type, data, d_size, start_col, end_col);
    ret.pos.start_line = start_line;
    return ret;
}

/* Token type is self-explanatory, the list is in the Tokens.h file
 * Data is a pointer to anything that may need to be stored e.g. for identifiers or keywords its a char* to
 * the word, for a number its a pointer to a 64 bit number, for a float a pointer to a double, ect
 * The data size is just for the malloc
 * start and end col e.g. 123, <1:3>
 */
Token create_token(TokenType type, void* data, uint64_t d_size, uint32_t start_col, uint32_t end_col) {
    Token t;

    t.type = type;
    t.pos = (Position){line_num, start_col, line_num, end_col};
    t.data = NULL;

    if (data == NULL) {
        return t;
    }

    t.data = malloc(d_size);
    memcpy(t.data, data, d_size);

    switch (type) {
        case IDENTIFIER:
        case KEYWORD:
        case TYPE:
        case LIT_BOOL:
        case LIT_STR:
            ((char*)t.data)[d_size - 1] = '\0';
            break;
        case LIT_INT:
        case LIT_FLOAT:
        case LIT_NAV:
        case OP_BIN:
        case OP_UN_PRE:
        case OP_UN_POST:
        case OP_TRINARY:
        case BRACKET_OPEN:
        case BRACKET_CLOSE:
        case CURLY_OPEN:
        case CURLY_CLOSE:
        case PAREN_OPEN:
        case PAREN_CLOSE:
        case COMMA:
        case TYPE_SET:
        case TYPE_INFER:
        case COMMENT:
        case DELIMITER:
        case WS_S:
        case WS_T:
        case NEWLINE:
        case EOTS:
            break;
    }

    return t;
}

void add_token(Token t) {
    Token_vec_add(tokens, t);
}

uint32_t current_char(void) {
    return consume_utf_char(c_char, false, NULL);
}

uint32_t peek(void) {
    char* current_end;
    consume_utf_char(c_char, false, &current_end);
    return consume_utf_char(current_end, false, NULL);
}

char* gourge(uint amount) {
    for (uint i = 0; i < amount; i++) consume();

    return c_char;
}

void update_line_count(void) {
    line_num++;
    col_num = 0;
}

char* consume_with_carridge(void) {
    if (*c_char == '\n') {
        update_line_count();

        if (!carridge_next_line()) return NULL;
        c_char = line_buffer.data;
        col_num++;
    } else {
        consume();
    }

    return c_char;
}

char* consume(void) {
    uint32_t character = consume_utf_char(c_char, true, NULL);

    if (character == '\n') {
        update_line_count();
    }

    col_num++;

    return c_char;
}

uint32_t consume_utf_char(char *start, bool consume, char** next_char) {
    uchar fbyte = *start;
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
                ((start[1] & 0x3f) << 6)       |
                (start[2] & 0x3f);
        inc = 3;
    }
    else if ((fbyte & 0xf8) == 0xf0 && (fbyte <= 0xf4)) {
        ret = ((fbyte & 0x07) << 18)            |
                ((start[1] & 0x3f) << 12)      |
                ((start[2] & 0x3f) << 6)       |
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

int carridge_next_line(void) {
    return get_line(c_file, &line_buffer);
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

void highlight_line_err(Position pos, const char* line) {
    int position_length = print_position(pos);
    printf("|\t\t%s", line);

    if (position_length < 0) goto skip_position_offset;
    for (uint i = 0; i < (uint)position_length; i++) {
        putchar(' ');
    }

    skip_position_offset:
    printf(" \t\t");

    for (uint i = 1; i <= pos.end_col; i++) {
        if (i < pos.start_col) putchar(' ');
        else printf(C_RED"^");
    }
    puts(C_RST);
}

void highlight_current_line_err(Position pos) {
    highlight_line_err(pos, line_buffer.data);
}
