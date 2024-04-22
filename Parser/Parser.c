//
// Created by jamescoward on 21/01/2024.
//

#include "Parser.h"

const Token_vec* ptokens;
const Vector* plines;
int t_pos;

Node file_global_node;

static void add_statement(Node* statement);

static NodeRet parse_statement(Token* t);
static NodeRet parse_statement_block(Token *token);

static NodeRet parse_keyword(Token* t);
static NodeRet parse_for_statement(Token* t);
static NodeRet parse_while_statement(Token* t);
static NodeRet parse_foreach_statement(Token* t);
static NodeRet parse_if_statement(Token* t);

static bool is_valid_index(int index);

static Token* confungry(int offset, bool consume, bool ignore_whitespace);
static Token* consume(void);
static Token* gourge(int amount);

static Token* current(void);
static Token* peek(void);
static Token* peer(int amount);

static uint parserr(ParsErrors errorCode, Token* parent_token, Token* issue_token, ...);

void add_statement(Node* statement) {
    if (statement == NULL) return;
    vector_add(&file_global_node.children, statement);
}

uint parse(const Token_vec *token_vec, const Node_vec *nodes, const Vector *lines) {
    ptokens = token_vec;
    plines = lines;
    t_pos = 0;

    uint ret_code = SUCCESS;

    Token* c;

    file_global_node = (Node){NODE_FILE, NULL, vector_create(MIN_CHILDREN)};

    while (c = current(), c != NULL && c->type != EOTS) {
        const NodeRet ret = parse_statement(c);

        if (ret.retCode != SUCCESS) {
            ret_code = ret.retCode;
        }

        add_statement(ret.node);
    }

    return ret_code;
}


NodeRet parse_statement(Token* t) {
    //[[todo]] could be a single statement or a block ('{')
    // this will help with having if (expr) statement
    // without braces
    switch (t->type) {
        case LIT_INT:
        case IDENTIFIER:
        case OP_UN_PRE:
        case PAREN_OPEN: {
            //[[todo]] this is temp!!
            ShuntRet shuntData = shunt(ptokens, t_pos, false);
            t_pos = shuntData.tok_end_pos;
            return (NodeRet){shuntData.expressionNode, shuntData.err_code};
        }


        case KEYWORD:
            return parse_keyword(t);

        case CURLY_OPEN:
            return parse_statement_block(NULL);

        case COMMENT:
        case WS_T:
        case WS_S:
        case NEWLINE:
            consume();
            return (NodeRet){NULL, SUCCESS};

        case TOKEN_INVALID:
        default:
            consume();
            return construct_error_node(t);
    }
}

NodeRet parse_statement_block(Token* token) {
    Node* parent_node = create_parent_node(ST_BLOCK, token);

    if (!parent_node) return (NodeRet){NULL, FAIL}; //[[todo]] add error for creation (malloc)

    consume(); //eat the '{'

    NodeRet child;
    while (child = parse_statement(current()), child.retCode == SUCCESS) {
        vector_add(&parent_node->children, child.node);
    }

    if (current()->type != CURLY_CLOSE) parserr(PARSERR_BLOCK_MISSING_BRACE,
                                                parent_node->token, current());

    return (NodeRet){parent_node, child.retCode};
}

NodeRet parse_keyword(Token* t) {
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
            printf(C_RED"KEYWORD: EVal: `%d` SVal: `%s`\n"C_RST, t->data.enum_pos, ATOM_CT__LEX_KEYWORDS_RAW[t->data.enum_pos]);
//            assert(false);
            consume();
        //[[todo]] throw error
            return construct_error_node(NULL);
    }
}

NodeRet parse_variable_decl_statement(Token* t) {
    //expects current() to be t.
    //and t to be the variable

    /* var could be:
     * var: type                --DECLARATION
     * var: type = expr         --DECLARATION + ASSIGNMENT
     * var := expr              --DECLARATION + ASSIGNMENT
     * var :: type = expr       --IMPLICIT CAST + DECL + ASSIGN
     */


}

NodeRet parse_variable_ass_statement(Token* t) {
    //     * var = expr               --ASSIGNMENT


}

NodeRet parse_variable_statement(Token* t) {
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
     *      always assume its a new variable, but this will undoubtedly be annoying
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



NodeRet parse_for_statement(Token* t) {
    /*
     * FOR [x] to [y] do [z] statement
     *
     * [x] can be [var] [var = expr] [var: type] [var: type = expr]
     * [y] can be [var] [var = expr] [var [op] expr] [expr]
     * [z] can be [expr]
     */

    consume();

    return construct_error_node(t);
}

NodeRet parse_while_statement(Token* t) {
    consume();

    return construct_error_node(t);
}

NodeRet parse_foreach_statement(Token* t) {
    consume();

    return construct_error_node(t);
}

NodeRet parse_if_statement(Token* t) {
    consume();

    return construct_error_node(t);
}

bool verify_token_for_print(Token* token) {
    return token && token->pos.start_line < plines->pos && token->pos.start_line == token->pos.end_line;
}

uint parserr(const ParsErrors errorCode, Token* parent_token, Token* issue_token, ...) {
    switch (errorCode) {
        case PARSERR_BLOCK_MISSING_BRACE:
            puts(ATOM_CT__PARSERR_BLOCK_MISSING_BRACE);
            break;
        default:
            assert(false);
    }

    if (verify_token_for_print(parent_token)) {
        highlight_line_err(parent_token->pos, plines->arr[parent_token->pos.start_line]);
    }

    if (verify_token_for_print(issue_token)) {
        highlight_line_err(issue_token->pos, plines->arr[issue_token->pos.start_line]);
    }
}

bool is_valid_index(int index) {
    return index >= 0 && index < ptokens->pos;
}

Token* confungry(int offset, bool consume, bool ignore_whitespace) {
    if (offset == 0) {
        return &ptokens->arr[t_pos];
    }

    if (!ignore_whitespace) {
        if (!is_valid_index(t_pos + offset)) {
            if (consume) {
                t_pos += offset;
            }
            return NULL;
        }

        if (consume) {
            t_pos += offset;
            return &ptokens->arr[t_pos];
        }

        return &ptokens->arr[t_pos + offset];
    }

    int valid_skipped = 0;
    int diff = offset < 0 ? -1 : 1;
    int current_offset = 0;

    while (valid_skipped < abs(offset)) {
        current_offset += diff;
        if (!is_valid_index(t_pos + current_offset)) {
            if (consume) {
                t_pos += current_offset;
            }
            return NULL;
        }

        if (is_whitespace_tkn(ptokens->arr[t_pos + current_offset].type)) {
            continue;
        }

        valid_skipped += 1;
    }

    if (consume) {
        t_pos += current_offset;
        return &ptokens->arr[t_pos];
    }

    return &ptokens->arr[t_pos + current_offset];
}

Token* peer(int amount) {
    return confungry(amount, false, true);
}

Token* peek(void) {
    return peer(1);
}

Token* justify(void) {
    return peer(-1);
}

Token* consume(void) {
    return confungry(1, true, true);
}

Token* gourge(const int amount) {
    return confungry(amount, true, true);
}

Token* current(void) {
    return confungry(0, false, false);
}
