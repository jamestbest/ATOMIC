//
// Created by jamescoward on 11/11/2023.
//

#include "Compiler.h"

VEC_ADD(Token, Token)

CompileRet compile(const char* entry_point, const char* out_format, charp_vec files) {
    Token_vec tokens = Token_vec_create(BUFF_SIZE);
    charp_vec lines = charp_vec_create(BUFF_SIZE);

    //for each file
    char* filename = charp_vec_get(&files, 0);
    FILE* fptr = fopen(filename, "r");

    if (fptr == NULL) {
        return (CompileRet) {ERR_NO_SUCH_FILE, filename};
    }

    lex(fptr, &tokens, &lines);

    return (CompileRet) {SUCCESS, NULL};
}