//
// Created by jamescoward on 21/01/2024.
//

#include "Parser.h"

const Token_vec* ptokens;
uint t_pos;

Node_vec* statements;

static void add_statement(Node statement);

static NodeRet parse_statement(const Token* t);
static NodeRet parse_keyword(const Token* t);

static NodeRet parse_for_statement(const Token* t);
static NodeRet parse_while_statement(const Token* t);
static NodeRet parse_foreach_statement(const Token* t);
static NodeRet parse_if_statement(const Token* t);

static Token* consume(void);
static Token* gourge(uint amount);

static Token* current(void);
static Token* peek(void);
static Token* peer(uint amount);

static NodeRet construct_error_node(void);

void add_statement(const Node statement) {
    Node_vec_add(statements, statement);
}

uint parse(const Token_vec* token_vec, const Vector* nodes) {
    ptokens = token_vec;
    t_pos = 0;

    uint ret_code = SUCCESS;

    assert(nodes->type == NODE);

    Token* c;
    while (c = current(), c != NULL && c->type != EOTS) {
        const NodeRet ret = parse_statement(c);

        if (ret.retCode != SUCCESS) {
            ret_code = ret.retCode;
        }
    }

    return ret_code;
}

NodeRet construct_error_node(void) {
    return (NodeRet){{NODE_INVALID, {NULL}}, FAIL};
}

NodeRet parse_statement(const Token* t) {
    //[[todo]] could be a single statement or a block ('{')
    // this will help with having if (expr) statement
    // without braces
    switch (t->type) {
        case KEYWORD:
            return parse_keyword(t);

        case TOKEN_INVALID:
        default:
            return construct_error_node();
    }
}

NodeRet parse_keyword(const Token* t) {
    switch ((ATOM_CT__LEX_KEYWORD_ENUM) t->data.enum_pos) {
        case FOR:
            return parse_for_statement(t);
        case WHILE:
            return parse_while_statement(t);
        case FOREACH:
            return parse_foreach_statement(t);
        case IF:
            return parse_if_statement(t);
        default:
            printf("%llu", t->data.enum_pos);
            assert(false);
            consume();
        //[[todo]] throw error
            return construct_error_node();
    }
}

NodeRet parse_variable_decl_statement(const Token* t) {
    //expects current() to be t.
    //and t to be the variable

    /* var could be:
     * var: type                --DECLARATION
     * var: type = expr         --DECLARATION + ASSIGNMENT
     * var := expr              --DECLARATION + ASSIGNMENT
     * var :: type = expr       --IMPLICIT CAST + DECL + ASSIGN
     */


}

NodeRet parse_variable_ass_statement(const Token* t) {

}

NodeRet parse_variable_statement(const Token* t) {
    /* var could be:
     * var: type                --DECLARATION
     * var: type = expr         --DECLARATION + ASSIGNMENT
     * var := expr              --DECLARATION + ASSIGNMENT
     * var :: type = expr       --IMPLICIT CAST + DECL + ASSIGN
     * var = expr               --ASSIGNMENT
     *
     * [[todo]] implicit cast&decl&assign's syntax means that it does not
     *      distinguish between declaring a variable and using one that already
     *      exists. There should probably be some kind of way to do this. For now
     *      always assume its a new variable, but this will undoubtbly be annoying
     *      Could have var ::= expr perhaps but this removes seeing the type its
     *      being cast to which is the whole point. Also looks like implicit type
     */

    Token* look_ahead = peek();

    switch (look_ahead->type) {
        case TYPE_SET:
            return parse_variable_decl_statement(t);
        // case
        default:
            break;
    }
}



NodeRet parse_for_statement(const Token* t) {
    /*
     * FOR [x] to [y] do [z] statement
     *
     * [x] can be [var] [var = expr] [var: type] [var: type = expr]
     * [y] can be [var] [var = expr] [var [op] expr] [expr]
     * [z] can be [expr]
     */



    return construct_error_node();
}

NodeRet parse_while_statement(const Token* t) {
    return construct_error_node();
}

NodeRet parse_foreach_statement(const Token* t) {
    return construct_error_node();
}

NodeRet parse_if_statement(const Token* t) {
    return construct_error_node();
}

Token* current(void) {
    return &ptokens->arr[t_pos];
}

Token* peer(const uint amount) {
    if (t_pos + amount >= ptokens->pos) {
        return NULL;
    }

    return &ptokens->arr[t_pos + amount];
}

Token* peek(void) {
    return peer(1);
}

Token* gourge(const uint amount) {
    for (uint i = 0; i < amount; ++i) {
        consume();
    }
    return &ptokens->arr[t_pos];
}

Token* consume(void) {
    if (t_pos++ == ptokens->pos) {
        return NULL;
    }
    return &ptokens->arr[t_pos];
}
