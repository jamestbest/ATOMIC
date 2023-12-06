//
// Created by jamescoward on 11/11/2023.
//

#include "Compiler.h"

VEC_ADD(Token, Token)

CompileRet compile(const char* entry_point, const char* out_format, const char* cwd, charp_vec files) {

    //for each file
    for (int i = 0; i < files.pos; i++) {
        char* filename = charp_vec_get(&files, i);

        FILE* fp = open_file(cwd, filename, "r");

        if (fp == NULL) {
            return (CompileRet) {ERR_NO_SUCH_FILE, filename};
        }

        compile_file(entry_point, out_format, fp);
    }

    return (CompileRet) {SUCCESS, NULL};
}

CompileRet compile_file(const char* entry_point, const char* out_format, FILE* fp) {
    Token_vec tokens = Token_vec_create(BUFF_SIZE);
    charp_vec lines = charp_vec_create(BUFF_SIZE);

    lex(fp, &tokens, &lines);

    return (CompileRet) {SUCCESS, NULL};
}