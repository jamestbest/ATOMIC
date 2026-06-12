//
// Created by jamescoward on 08/06/2026.
//

#include "Parser.h"

#include "Lexer/Lexer.h"
#include "Parser2/Node.h"
#include "SharedIncludes/Vector.h"
#include "Parser2/Parserr.h"

const tokenArray* ptokens= NULL;
const Vector* plines= NULL;

size_t t_pos;

static Token* current();
static Token* expect(TokenType type);
static Token* peer(uint amount);
static Token* peek();
static Token* consume();
static Token* expect_keyword_any(size_t count, ...);
static Token* expect_keyword(ATOM_CT__LEX_KEYWORD_ENUM kw);
static Token* expect_any(size_t count, ...);

static NodeRet parse_top_level(const Token* c);
static NodeRet parse_subroutine(bool is_func);

NodeRet parse(const tokenArray* tokens, const Vector* lines) {
    ptokens= tokens;
    plines= lines;

    t_pos= 0;

    uint ret_code= SUCCESS;

    init_nodes();
    RootNode* root= create_root_node();

    Token* c;
    while (c= current(), c != NULL) {
        const NodeRet res= parse_top_level(c);

        if (res.res != SUCCESS) break;

        vector_add(&root->tl_statements, res.node);
    }

    return PARSE_SUCC(root);
}

static NodeRet parse_top_level(const Token* c) {
    /* This is the top level
     *  - Global variable declaration
     *  - Type declaration (struct)
     *  - function declaration/definition
     */
    switch (c->type) {
        case IDENTIFIER: return parse_var_declaration();
        case KEYWORD: {
            const ATOM_CT__LEX_KEYWORD_ENUM kw= c->data.enum_pos;

            switch (kw) {
                case FUNC:
                case PROC: return parse_subroutine(kw == FUNC);
                default: break;
            }
        }
        default: break;
    }

    return parser_error(
        "Top level statements must be either a global variable declaration, function declaration, or struct definition",
        NULL,
        c
    );
}

static NodeRet parse_parameter(const Token* parent_token) {

}

static NodeRet parse_subroutine(bool is_func) {
    /* Subroutine types
     * func <name>(<args>): <type> {}
     * proc <name>(<args>)         {}
     */
    const Token* keyword= expect_keyword_any(FUNC, PROC);
    assert(keyword);

    const Token* identifier= expect(IDENTIFIER);
    if (!identifier) return parser_unexpected(
        "parsing subroutine name",
        IDENTIFIER,
        current()
    );

    if (!expect(PAREN_OPEN)) return parser_unexpected(
        "parsing subroutine parameter list",
        PAREN_OPEN,
        current()
    );

    ParamVector params= Param_vec_construct(0);
    const Token* c;
    while (c= consume(), c->type != PAREN_CLOSE) {
        const NodeRet param= parse_parameter(identifier);
        if (param.res != SUCCESS) return param;

        Param_vec_add(&params, param.node);
    }

    if (is_func) {
        if (!expect(TYPE_SET)) {
            parser_unexpected(
                "function after parameters",
                TYPE_SET,
                current()
            );

            parser_fix(
                c,
                PFO_AFTER,
                PFT_ADDITION,
                ": <type>",
                "functions must have a return type, specified by ':' after the arguments"
            );
            parser_fix_or();
            parser_fix(
                keyword,
                PFO_NONE,
                PFT_REPLACE,
                "proc",
                "procedures do not have a return value"
            );
            return PARSE_FAIL;
        }
    }
}

Token* current() {
    if (t_pos >= ptokens->pos) return NULL;

    return token_arr_ptr(ptokens, t_pos);
}

Token* consume() {
    if (t_pos >= ptokens->pos) return NULL;

    return token_arr_ptr(ptokens, t_pos++);
}

Token* peek() {
    if (t_pos + 1 >= ptokens->pos) return NULL;

    return token_arr_ptr(ptokens, t_pos + 1);
}

Token* peer(uint amount) {
    if (t_pos + amount >= ptokens->pos) return NULL;

    return token_arr_ptr(ptokens, t_pos + amount);
}

Token* expect(TokenType type) {
    Token* c= current();
    if (!c) return NULL;
    if (c->type != type) return NULL;

    return consume();
}

Token* expect_keyword(ATOM_CT__LEX_KEYWORD_ENUM kw) {
    Token* c= expect(KEYWORD);
    if (!c) return NULL;
    if (c->data.enum_pos != kw) return NULL;

    return consume();
}

Token* expect_any(size_t count, ...) {
    va_list args;
    va_start(args, count);

    const Token* c= current();
    for (size_t i= 0; i < count; ++i) {
        TokenType type= va_arg(args, TokenType);
        if (c->type == type) {
            va_end(args);
            return consume();
        }
    }
    va_end(args);

    return NULL;
}

Token* expect_keyword_any(const size_t count, ...) {
    const Token* c= current();
    if (c->type != KEYWORD) return NULL;

    va_list args;
    va_start(args, count);

    for (size_t i= 0; i < count; ++i) {
        const ATOM_CT__LEX_KEYWORD_ENUM kw= va_arg(args, ATOM_CT__LEX_KEYWORD_ENUM);
        if (c->data.enum_pos == kw) {
            va_end(args);
            return consume();
        }
    }
    va_end(args);

    return NULL;
}
