//
// Created by james on 28/10/23.
//

#include "Lexer.h"

void lex(FILE* file, Token_vec* tokens, charp_vec* lines) {
    /*
     * Need to read in the file and turn the strings into tokens
     */

    /* Streaming tokens? not at the moment just store all the lines in a buffer and then store information on the position and size of a token */
    Buffer buffer = buffer_create(BUFF_SIZE);

    while (get_line(file, &buffer)) {
        line_to_tokens(&buffer, tokens);

        char* line = buffer_steal(&buffer, BUFF_SIZE);
        charp_vec_add(lines, line);
    }

    buffer_destroy(&buffer);
}

enum test {
    TEST,
    TEST2
};

struct test2 {
    void* data;
};

/*
 * convert a char* to vec of tokens
 *
 * Words - Identifiers, Keywords,
 * Numbers - Floats/Ints
 * Strings/chars
 * Operators
 */
void line_to_tokens(Buffer* line, Token_vec* tokens) {
    struct test2 t = {(void *) TEST};

    printf("%d", (enum test)t.data);
}