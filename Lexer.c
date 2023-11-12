//
// Created by james on 28/10/23.
//

#include "Lexer.h"

VEC_ADD(Token, Token)

Token_vec lex(FILE* file) {
    /*
     * Need to read in the file and turn the strings into tokens
     * I don't want to read the whole file in
     */
    Token_vec tokens = Token_vec_create(BUFF_SIZE);

    /* Streaming tokens? not at the moment just store all the lines in a buffer and then store information on the position and size of a token */
    charp_vec lines = charp_vec_create(BUFF_SIZE);

    char_vec buffer = char_vec_create(BUFF_SIZE);

    while (get_line(file, &buffer)) {
        line_to_tokens(&buffer, &tokens);

        char* line = char_vec_steal(&buffer);
        charp_vec_add(&lines, line);
    }

    char_vec_destroy(&buffer);

    return tokens;
}


/*
 * convert a char* to vec of tokens
 *
 * Words - Identifiers, Keywords,
 * Numbers - Floats/Ints
 * Strings/chars
 * Operators
 */
void line_to_tokens(char_vec* line, Token_vec* tokens) {

}