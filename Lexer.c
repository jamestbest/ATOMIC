//
// Created by james on 28/10/23.
//

#include "Lexer.h"

Token* lex(FILE* file) {
    /* Need to read in the file and turn the strings into tokens
     * I don't want to read the whole file in
     */

    Token_vec tokens = Token_vec_create();

    size_t buff_max = BUFF_SIZE;
    /* Streaming tokens? not at the moment just store all the lines in a buffer and then store information on the position and size of a token */
    charp_vec lines = charp_vec_create();

    char* buff = malloc(buff_max * sizeof(char)); //store at least 100 characters

    while (get_next_line(&buff, &buff_max, file)) {
        //buff should contain the entire line as getline will resize the buffer to accommodate

        char* line = malloc(buff_max);

        if (line == NULL) {
            perror("Error: Malloc call within lexing");
            exit(errno);
        }

        memcpy(line, buff, buff_max);

        charp_vec_add(&lines, line);

        //[[maybe]] downsize the buffer to reduce the amount of memcpy otherwise a long line is going to increase the size of the buffer and all
        //  subsequent runs will have to copy the larger buffer.
    }

    free(buff);

    return 0;
}



size_t get_next_line(char** buff, size_t* buff_max, FILE* file) {
    size_t ret;

    ret = getline(buff, buff_max, file);

    assert(errno != EINVAL);

    if (errno == ENOMEM) {
        printf("Error: OUT OF MEM L bozo");
        exit(ENOMEM);
    }

    if (ret == -1) {
        perror("Error: Failed to read new line in file");
    }

    return ret;
}