//
// Created by jamescoward on 11/11/2023.
//

#include "Compiler.h"

VEC_ADD(Token, Token)
VEC_ADD(Node, Node)

//[[todo]] better to use something like a block allocator(? is that the name)
// where a large part of memory is allocated and then the data for the tokens
// and nodes are stored there instead of having then spread out with
// rounding of allocation. They would also all have the same life time.

CompileRet compile(const char* entry_point, const char* out_format, const char* cwd, charp_vec files) {

    assert(ATOM_CT__LEX_KEYWORD_ENUM_COUNT == ATOM_CT__LEX_KEYWORDS.elem_count);
    assert(strcmp(ATOM_CT__LEX_KEYWORDS_RAW[IF], "if") == 0);

    //for each file
    for (uint i = 0; i < files.pos; i++) {
        const char* filename = charp_vec_get(&files, i);

        FILE* fp = open_file(cwd, filename, "r");

        if (fp == NULL) {
            return (CompileRet) {ERR_NO_SUCH_FILE, filename};
        }

        CompileRet ret = compile_file(entry_point, out_format, fp);

        if (ret.code != SUCCESS) return ret;
    }

    return (CompileRet) {SUCCESS, NULL};
}

CompileRet compile_file(const char* entry_point, const char* out_format, FILE* fp) {
    Token_vec tokens = Token_vec_create(BUFF_SIZE);
    //this is an array of structs, could become a struct of arrays:
    //Is it more likely that the data of consecutive structs is accessed
    //Or that the (data/type) of a structure is access more? - Printing will do this
    //For now it will stay as AOS

    Vector lines = vector_create(BUFF_MIN);

    uint lexRet = lex(fp, &tokens, &lines);

    print_tokens_with_flag_check(&tokens, &lines);

    if (lexRet != SUCCESS) {
        free_tokens(&tokens);

        return (CompileRet) {LEXERR, NULL};
    }

    //parse
    Node_vec nodes = Node_vec_create(BUFF_MIN);

    uint parseRet = parse(&tokens, &nodes, &lines);

    if (parseRet != SUCCESS) {
        free_nodes(&nodes);
        free_tokens(&tokens); //[[todo]] do the nodes take over control & responsibility of the tokens

        return (CompileRet){PARSERR, NULL};
    }

    //...
    vector_disseminate_destruction(&lines);
    free_tokens(&tokens);

    return (CompileRet) {SUCCESS, NULL};
}

void free_tokens(Token_vec* tokens) {
    for (uint i = 0; i < tokens->pos; i++) {
        if (type_needs_free(tokens->arr[i].type)) {
            free(tokens->arr[i].data.ptr);
        }
    }
    Token_vec_destroy(tokens);
}

void free_nodes(Node_vec* nodes) {
    Node_vec_destroy(nodes);
}

void print_tokens_with_flag_check(Token_vec* tokens, Vector* lines) {
    if (flag_get(ATOM_CT__FLAG_VLTOK_OUT)) {
        print_verbose_tokens(tokens, lines, true);
        return;
    }
    if (flag_get(ATOM_CT__FLAG_VTOK_OUT)) {
        print_verbose_tokens(tokens, lines, false);
        return;
    }
    if (flag_get(ATOM_CT__FLAG_TOK_OUT)) {
        print_tokens(tokens, false, false); //[[todo]] have white space and comments included in flag set
        return;
    }
}
