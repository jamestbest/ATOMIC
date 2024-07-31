//
// Created by jamescoward on 11/11/2023.
//

#include "Compiler.h"

#include "Lexer/OpFolder.h"
#include "SharedIncludes/Helper_File.h"

#include "SharedIncludes/Array.h"

#include "Parser/StaticVerification.h"

/*  TODO
 *      UPDATE ALL VECTORs TO VECs
 *      CHECK WHAT THE ISSUE WITH hadron is
 *      HF
 */

ARRAY_ADD(Token, token)
ARRAY_ADD(Node, node)

static void free_tokens(Array* tokens);

//[[todo]] better to use something like a block allocator(? is that the name) (ARENA ALLOC?)
// where a large part of memory is allocated and then the data for the base_tokens
// and nodes are stored there instead of having then spread out with
// rounding of allocation. They would also all have the same life time.

//[[TODO]] IF ANYTHING FROM AN ARRAY IS REMOVED ALL EXTERNAL POINTERS WILL BE OFFSET INCORRECTLY

CompileRet compile(const char* entry_point, const char* out_format, const char* cwd, Vector files) {

    assert(ATOM_CT__LEX_KEYWORD_ENUM_COUNT == ATOM_CT__LEX_KEYWORDS.elem_count);
    assert(strcmp(ATOM_CT__LEX_KEYWORDS_RAW[IF], "if") == 0);

    //for each file
    for (uint i = 0; i < files.pos; i++) {
        const char* filename = vector_get_unsafe(&files, i);

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
    Array base_tokens = arr_create(sizeof (Token));
    Array folded_tokens;
    // [[maybe]] this is an array of structs, could become a struct of arrays:
    //  Is it more likely that the data of consecutive structs is accessed
    //  Or that the (data/type) of a structure is access more? - Printing will do this
    //  For now it will stay as AOS

    Vector lines = vector_create(BUFF_MIN);

    uint lexRet = lex(fp, &base_tokens, &lines);
    print_tokens_with_flag_check(&base_tokens, &lines, "\n\nBASE TOKENS");
    fflush(stdout);

    if (lexRet != SUCCESS) {
        free_tokens(&base_tokens);

        return (CompileRet) {LEXERR, NULL};
    }

    folded_tokens = arr_construct(sizeof (Token), base_tokens.pos);
    uint foldRet = fold(&base_tokens, &folded_tokens);
    // this is here for debug purposes, it should be after error checking in later versions
    print_tokens_with_flag_check(&folded_tokens, &lines, "\n\nFOLDED TOKENS");
    fflush(stdout);

    // we no longer need the base tokens BUT the token data inside is now under the control of the folded tokens
    // we shouldn't call free_tokens unless something went wrong, otherwise we will double free the data
    arr_destroy(&base_tokens);

    if (foldRet != SUCCESS) {
        free_tokens(&folded_tokens);

        return (CompileRet) {LEXERR, NULL};
    }

    //parse
    NodeRet parseRet = parse(&folded_tokens, &lines);

    if (parseRet.retCode != SUCCESS) {
        free_tokens(&folded_tokens); //[[todo]] do the nodes take over control & responsibility of the base_tokens -- I think not

        return (CompileRet){PARSERR, NULL};
    }

    print_ast_with_flag_check(parseRet.node);

    Scope* global_scope = generate_global_scope(parseRet.node);

    if (flag_get(ATOM_CT__FLAG_SCOPE_OUT))
        print_scope(global_scope);

    verify_scope(parseRet.node, global_scope, parseRet.node);

    print_ast_with_flag_check(parseRet.node);

    // verify_types(parseRet.node, global_scope, parseRet.node);

    //...
    vector_disseminate_destruction(&lines);
    free_tokens(&folded_tokens);

    free_node_rec(parseRet.node);

    return (CompileRet) {SUCCESS, NULL};
}

void free_tokens(Array* tokens) {
    for (uint i = 0; i < tokens->pos; i++) {
        const Token t = token_arr_get(tokens, i);
        if (type_needs_free(t.type)) {
            free(t.data.ptr);
        }
    }
    arr_destroy(tokens);
}

void print_ast_with_flag_check(Node* tl_node) {
    if (flag_get(ATOM_CT__FLAG_AST_OUT)) {
        print_top_level_node(tl_node);
    }
}

void print_tokens_with_flag_check(Array* tokens, Vector* lines, const char* print_header) {
    const bool vltok = flag_get(ATOM_CT__FLAG_VLTOK_OUT);
    const bool vtok = flag_get(ATOM_CT__FLAG_VTOK_OUT);
    const bool tok = flag_get(ATOM_CT__FLAG_TOK_OUT);

    if (vltok || vtok || tok) {
        puts(print_header);
    }

    if (vltok) {
        print_verbose_tokens(tokens, lines, true);
        return;
    }
    if (vtok) {
        print_verbose_tokens(tokens, lines, false);
        return;
    }
    if (tok) {
        print_tokens(tokens, false, false); //[[todo]] have white space and comments included in flag set
        return;
    }
}