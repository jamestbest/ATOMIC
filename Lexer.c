//
// Created by james on 28/10/23.
//

#include "Lexer.h"

void lex(FILE* file, Token_vec* tokens, charp_vec* lines) {
    /*
     * Need to read in the file and turn the strings into tokens
     */

    /* Streaming tokens? not at the moment just store all the lines in a buffer and then store information on the position and size of a token */
    char_vec buffer = char_vec_create(BUFF_SIZE);

    while (get_line(file, &buffer)) {
        line_to_tokens(&buffer, tokens);

        char* line = char_vec_steal(&buffer);
        charp_vec_add(lines, line);
    }

    char_vec_destroy(&buffer);
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