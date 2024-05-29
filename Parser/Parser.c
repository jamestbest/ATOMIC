//
// Created by jamescoward on 21/01/2024.
//

#include "Parser.h"

const Token_vec* ptokens;
const Vector* plines;
int t_pos;

Node* file_global_node;

static void add_statement(Node* statement);

static NodeRet parse_statement(void);
static NodeRet parse_statement_block(void);

static NodeRet parse_identifier_statement(void);

static NodeRet parse_keyword(void);
static NodeRet parse_ret_statement(void);
static NodeRet parse_for_statement(void);
static NodeRet parse_while_statement(void);
static NodeRet parse_foreach_statement(void);
static NodeRet parse_times_statement(void);
static NodeRet parse_if_statement(void);
static NodeRet parse_func_statement(void);
static NodeRet parse_proc_statement(void);
static NodeRet parse_entry_statement(void);
static NodeRet parse_continue_statement(void);
static NodeRet parse_break_statement(void);

static NodeRet parse_subroutine_call(void);

static NodeRet parse_un_op_statement(void);

static bool is_valid_index(int index);

static Token* confungry(int offset, bool consume, bool ignore_whitespace,
                        bool ignore_newline);
static Token* consume(void);

static Token* current(void);
static Token* peek(void);
static Token* peer(int amount);

static bool expect_op(TokenType type, ATOM_CT__LEX_OPERATORS_ENUM op);
static bool expect_keyword(ATOM_CT__LEX_KEYWORD_ENUM keyword);
static bool expect(TokenType type);

static NodeRet parserr(ParsErrors errorCode, Token* parent_token, Token* issue_token, ...);

// [[todo]] I've just realised that the Node* created are leaked if there are error!

void add_statement(Node* statement) {
    if (statement == NULL) return;

//    assert(statement->type); //[[todo]] temp dev

    if (statement->type == NODE_MULTIPLE_STATEMENTS) {
        for (uint i = 0; i < statement->children.pos; ++i) {
            Node* stmt = statement->children.arr[i];
            vector_add(&file_global_node->children, stmt);
        }
        free_node(statement);
    } else {
        vector_add(&file_global_node->children, statement);
    }
}

NodeRet parse(const Token_vec* token_vec, const Vector* lines) {
    ptokens = token_vec;
    plines = lines;
    t_pos = 0;

    uint ret_code = SUCCESS;

    Token* c;

    file_global_node = create_parent_node(NODE_ROOT, NULL);

    while (c = current(), c != NULL && c->type != EOTS) {
        const NodeRet ret = parse_statement();

        if (ret.retCode != SUCCESS) {
            ret_code = ret.retCode;
        }

        add_statement(ret.node);
    }

    puts("END NODE");
    print_top_level_node(file_global_node);

    return (NodeRet){file_global_node, ret_code};
}

bool is_skippable(TokenType type) {
    return type == DELIMITER || type == NEWLINE;
}

void skip_all_skippables(void) {
    while (current() && is_skippable(current()->type)) {
        consume();
    }
}

NodeRet parse_statement(void) {
    //[[todo]] could be a single statement or a block ('{')
    // this will help with having if (expr) statement
    // without braces
    NodeRet ret;
    Token* t;

    skip_all_skippables();

    t = current();

    switch (t->type) {
        case LIT_INT:
        case OP_UN_PRE:
        case PAREN_OPEN: {
            //[[todo]] this is temp!!
            ShuntRet shuntData = shunt(ptokens, t_pos, false);
            t_pos = shuntData.tok_end_pos;
            ret = (NodeRet){shuntData.expressionNode, shuntData.err_code};
            break;
        }

        case IDENTIFIER:
            ret = parse_identifier_statement();
            break;

        case KEYWORD:
            ret = parse_keyword();
            break;

        case CURLY_OPEN:
            ret = parse_statement_block();
            break;

        case TOKEN_INVALID:
        default:
            consume();
            ret = construct_error_node(t);
            break;
    }

    skip_all_skippables();

    if (ret.retCode != SUCCESS) {
        assert(false);
    }

    return ret;
}

NodeRet parse_subroutine_call(void) {
    Token* identifier = current();

    ShuntRet sub_call = shunt(ptokens, t_pos, false);

    if (sub_call.err_code != SUCCESS) {
        return parserr(PARSERR_SUB_CALL_PARSE_ERROR, current(), NULL);
    }

    if (sub_call.expressionNode->type != SUB_CALL) {
        return parserr(PARSERR_SUB_CALL_EXPECTED_GOT_EXPR, identifier, NULL);
    }

    t_pos = sub_call.tok_end_pos;

    return (NodeRet){sub_call.expressionNode, SUCCESS};
}

NodeRet parse_var_assignment(void) {
    /* This could be from
     *  a = 12
     *  a += 12
     *  THIS WILL BE HANDLED SEPARATELY b::i4 = 12 + a  --implicit cast
     */
    Token* identifier = consume();

    Node* assignNode = create_parent_node(ST_VAR_ASS, identifier);

    if (!expect(ASSIGN) && !expect(ARITH_ASSIGN)) {
        assert(false);
    }

    Token* operator = consume(); // eat `=` or `+=`

    ShuntRet assign_value = shunt(ptokens, t_pos, false);

    if (assign_value.err_code != SUCCESS) {
        assert(false);
    }
    t_pos = assign_value.tok_end_pos;

    Node* assign_expr = assign_value.expressionNode;

    vector_add(&assignNode->children, create_leaf_node(TOKEN_WRAPPER, identifier));
    vector_add(&assignNode->children, create_leaf_node(TOKEN_WRAPPER, operator));
    vector_add(&assignNode->children, assign_expr);

    return (NodeRet){assignNode, SUCCESS};
}

NodeRet parse_var_declaration(void) {
    /* An identifier could be
     *  a : i4 =?           // var decl/assignment
     *  b :: i4 =           // var decl+assignment
     */

    Token* identifier = consume();
    Token* type_op = consume();

    if (!expect(TYPE))
        assert(false);

    Token* type = consume();

    Node* declNode = create_parent_node(ST_VAR_DECL, type_op);

    vector_add(&declNode->children, create_leaf_node(TOKEN_WRAPPER, identifier));
    vector_add(&declNode->children, create_leaf_node(TOKEN_WRAPPER, type));

    if (!expect(ASSIGN)) {
        return (NodeRet){declNode, SUCCESS};
    }

    Token* assign = consume(); // eat the `=`
    Node* ret = create_parent_node(NODE_MULTIPLE_STATEMENTS, NULL);
    Node* assignNode = create_parent_node(ST_VAR_ASS, assign);

    vector_add(&ret->children, declNode);
    vector_add(&ret->children, assignNode);

    ShuntRet exprInfo = shunt(ptokens, t_pos, false);

    if (exprInfo.err_code != SUCCESS)
        assert(false);
    t_pos = exprInfo.tok_end_pos;

    vector_add(&assignNode->children, exprInfo.expressionNode);

    return (NodeRet){ret, SUCCESS};
}

NodeRet parse_identifier_statement(void) {
    /* An identifier could be
     *  a : i4 =?           // var decl/assignment
     *  b :: i4 =           // var decl+assignment
     *  a()                 // function call
     *  a = b               // var assignment
     *  a += b              // arith assignment
     *  a++                 // un post op
     */
    Token* identifier = current();
    Token* next = peek();

    switch (next->type) {
        case TYPE_SET:
        case TYPE_IMPL_CAST:
            return parse_var_declaration();
        case PAREN_OPEN:
            return parse_subroutine_call();
        case ASSIGN:
        case ARITH_ASSIGN:
            return parse_var_assignment();
        case OP_UN_POST:
            return parse_un_op_statement();
        default:
            return parserr(PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT, identifier, next);
    }
}

NodeRet parse_statement_block(void) {
    Token* token = current();

    Node* parent_node = create_parent_node(ST_BLOCK, token);

    if (!parent_node) return (NodeRet){NULL, FAIL}; //[[todo]] add error for creation (malloc)

    if (!expect(CURLY_OPEN)) {
        assert(false); // currently most check for this before calling, but it should be unified
    }

    consume(); //eat the '{'

    NodeRet child;
    skip_all_skippables(); // This is unfortunately because it could be an empty block, and so parse statement would not be called to do this
    if (current()->type == CURLY_CLOSE) {
        // this is an empty statement block
        consume(); // eat '}'

        return (NodeRet){parent_node, SUCCESS};
    }

    while (child = parse_statement(), child.retCode == SUCCESS) {
        vector_add(&parent_node->children, child.node);

        if (current() && current()->type == CURLY_CLOSE)
            break;
    }

    if (current()->type != CURLY_CLOSE) {
        return parserr(PARSERR_BLOCK_MISSING_BRACE, parent_node->token, current());
    }

    consume(); // eat the CURLY_CLOSE

    return (NodeRet){parent_node, child.retCode};
}

NodeRet parse_keyword(void) {
    Token* t = current();

    switch ((ATOM_CT__LEX_KEYWORD_ENUM) t->data.enum_pos) {
        case FOR:
            return parse_for_statement();
        case WHILE:
            return parse_while_statement();
        case FOREACH:
            return parse_foreach_statement();
        case TIMES:
            return parse_times_statement();
        case IF:
            return parse_if_statement();
        case RET:
            return parse_ret_statement();
        case FUNC:
            return parse_func_statement();
        case PROC:
            return parse_proc_statement();
        case ENTRY:
            return parse_entry_statement();
        case CONT:
            return parse_continue_statement();
        case BRK:
            return parse_break_statement();
        default:
            printf(C_RED"KEYWORD: EVal: `%d` SVal: `%s`\n"C_RST, t->data.enum_pos, ATOM_CT__LEX_KEYWORDS_RAW[t->data.enum_pos]);
//            assert(false);
            consume();
        //[[todo]] throw error
            return construct_error_node(NULL);
    }
}

NodeRet parse_variable_decl_statement(void) {
    //expects current() to be t.
    //and t to be the variable

    /* var could be:
     * var: type                --DECLARATION
     * var: type = expr         --DECLARATION + ASSIGNMENT
     * var := expr              --DECLARATION + ASSIGNMENT
     * var :: type = expr       --IMPLICIT CAST + DECL + ASSIGN
     */


}

NodeRet parse_variable_ass_statement(void) {
    //     * var = expr               --ASSIGNMENT


}

NodeRet parse_variable_statement(void) {
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
            return parse_variable_decl_statement();
        // case
        default:
            break;
    }
}

NodeRet parse_param(void) {
    if (!expect(IDENTIFIER)) {
        assert(false);
    }

    Token* identifier = consume();

    if (!expect(TYPE_SET)) {
        assert(false);
    }

    consume(); // eat `:`

    if (!expect(TYPE)) {
        assert(false);
    }

    Token* type = consume();

    Node* paramNode = create_parent_node(SUB_PARAM, identifier);

    vector_add(&paramNode->children, create_leaf_node(TOKEN_WRAPPER, identifier));
    vector_add(&paramNode->children, create_leaf_node(TOKEN_WRAPPER, type));

    return (NodeRet){paramNode, SUCCESS};
}

NodeRet parse_params(void) {
    if (!expect(PAREN_OPEN)) {
        assert(false);
    }

    consume(); // eat `(`

    Node* params = create_parent_node(SUB_PARAMS, NULL);

    while (expect(IDENTIFIER)) {
        NodeRet param = parse_param();

        if (param.retCode != SUCCESS) {
            assert(false);
        }

        vector_add(&params->children, param.node);

        if (!expect(COMMA))
            break;

        consume(); // eat the `,`
    }

    if (!expect(PAREN_CLOSE)) {
        assert(false);
    }

    consume(); // eat the `)`

    return (NodeRet){params, SUCCESS};
}

NodeRet parse_sub_statement(bool isFunc) {
    Token* keyword = consume();

    Node* funcNode = create_parent_node(ST_FUNC, keyword);

    //[[todo]] for now just assuming there are no keyword modifiers

    if (!expect(IDENTIFIER)) {
        assert(false);
    }

    Token* func_name = consume();

    NodeRet params = parse_params();

    if (params.retCode != SUCCESS) {
        assert(false);
    }

    Token* type;
    if (isFunc) {
        if (!expect(TYPE_SET)) {
            assert(false);
        }

        consume(); // eat `:`

        if (!expect(TYPE)) {
            assert(false);
        }

        type = consume();
    }

    if (!expect(CURLY_OPEN)) {
        assert(false);
    }

    NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        assert(false);
    }

    vector_add(&funcNode->children, create_leaf_node(TOKEN_WRAPPER, func_name));
    vector_add(&funcNode->children, params.node);
    if (isFunc) vector_add(&funcNode->children, create_leaf_node(TOKEN_WRAPPER, type));
    vector_add(&funcNode->children, body.node);

    return (NodeRet){funcNode, SUCCESS};
}

NodeRet parse_func_statement(void) {
    return parse_sub_statement(true);
}

NodeRet parse_proc_statement(void) {
    return parse_sub_statement(false);
}

NodeRet parse_entry_statement(void) {
    Token* keyword = consume(); // eat the `entry`

    NodeRet subroutine;

    if (expect_keyword(FUNC)) {
        subroutine = parse_func_statement();
    } else if (expect_keyword(PROC)) {
        subroutine = parse_proc_statement();
    } else {
        assert(false);
    }

    // [[todo]] add to something the fact that this is an entry function

    return subroutine;
}

NodeRet parse_continue_statement(void) {
    // for now the continue statement will just be a keyword
    // but I would imagine that having labels / a way to continue a different loop

    Node* contNode = create_parent_node(ST_CONT, consume());

    return (NodeRet){contNode, SUCCESS};
}

NodeRet parse_break_statement(void) {
    // for now the break statement will just be a keyword
    // but I would imagine that having labels / a way to continue a different loop

    Node* brkNode = create_parent_node(ST_BRK, consume());

    return (NodeRet){brkNode, SUCCESS};
}

NodeRet parse_ret_statement(void) {
    Token* t = consume(); //eat the ret keyword

    Node* retNode = create_parent_node(ST_RET, t);

    ShuntRet exprData = shunt(ptokens, t_pos, false);

    if (exprData.err_code != SUCCESS) {
        assert(false);
    }

    t_pos = exprData.tok_end_pos;
    vector_add(&retNode->children, exprData.expressionNode);

    return (NodeRet){retNode, SUCCESS};
}

NodeRet parse_for_setup(void) {
    // assume start is one after `for` keyword
    //  can be [var] [var = expr] [var: type] [var: type = expr]

    if (!expect(IDENTIFIER)) {
        assert(false);
    }

    Token* identifier = consume();

    Token* c = current();

    //[[todo]] do

    switch (c->type) {
        case ASSIGN: {
            consume(); // eat the `=`

            ShuntRet expr = shunt(ptokens, t_pos, false);

            if (expr.err_code != SUCCESS) {
                assert(false);
            }
            t_pos = expr.tok_end_pos;

            Node* assignNode = create_parent_node(ST_VAR_ASS, identifier);
            vector_add(&assignNode->children, create_leaf_node(TOKEN_WRAPPER, identifier));
            vector_add(&assignNode->children, expr.expressionNode);

            return (NodeRet){assignNode, SUCCESS};
        }
        case TYPE_SET: {
            consume(); // eat the ':'

            if (!expect(TYPE)) {
                assert(false);
            }

            Token* type = consume();

            Node* varDeclNode = create_parent_node(ST_VAR_DECL, identifier);
            vector_add(&varDeclNode->children, create_leaf_node(TOKEN_WRAPPER, identifier));
            vector_add(&varDeclNode->children, create_leaf_node(TOKEN_WRAPPER, type));

            if (!expect(ASSIGN)) {
                // in this case it's just a:type, and so we will assume it is set to 0
                // [[todo]] but where do we assume this??, I'm going to say later, just so I don't have to generate tokens
                return (NodeRet){varDeclNode, SUCCESS};
            }

            Node* ret = create_parent_node(NODE_MULTIPLE_STATEMENTS, NULL);
            vector_add(&ret->children, varDeclNode);

            ShuntRet expr = shunt(ptokens, t_pos, false);

            if (expr.err_code != SUCCESS) {
                assert(false);
            }
            t_pos = expr.tok_end_pos;

            Node* varAssignNode = create_parent_node(ST_VAR_ASS, identifier);
            vector_add(&varAssignNode->children, create_leaf_node(TOKEN_WRAPPER, identifier));
            vector_add(&varAssignNode->children, expr.expressionNode);

            vector_add(&ret->children, varAssignNode);

            return (NodeRet){ret, SUCCESS};
        }
        default:
            // assume it's just an identifier
            return (NodeRet){create_leaf_node(EX_LIT, identifier), SUCCESS};
    }
}

NodeRet parse_for_cond(void) {
    Token* next = peek();
    if (next && next->type == ASSIGN) {
        if (!expect(IDENTIFIER)) {
            assert(false);
        }

        Token* identifier = consume();

        consume(); // eat `=`

        ShuntRet expr = shunt(ptokens, t_pos, false);

        if (expr.err_code != SUCCESS) {
            assert(false);
        }

        t_pos = expr.tok_end_pos;

        Node* assignNode = create_parent_node(ST_VAR_ASS, identifier);
        vector_add(&assignNode->children, create_leaf_node(TOKEN_WRAPPER, identifier));
        vector_add(&assignNode->children, expr.expressionNode);

        return (NodeRet){assignNode, SUCCESS};
    }

    ShuntRet expr = shunt(ptokens, t_pos, true); // we want to ignore trailing because `do` is optional and this could be in parens

    if (expr.err_code != SUCCESS) {
        assert(false);
    }

    t_pos = expr.tok_end_pos;

    return (NodeRet){expr.expressionNode, SUCCESS};
}

NodeRet parse_for_change(void) {
    ShuntRet expr = shunt(ptokens, t_pos, true);

    if (expr.err_code != SUCCESS) {
        assert(false);
    }
    t_pos = expr.tok_end_pos;

    return (NodeRet){expr.expressionNode, SUCCESS};
}

NodeRet parse_for_statement(void) {
    /*
     * FOR [x] to [y] do [z] statement
     *
     * [x] can be [var] [var = expr] [var: type] [var: type = expr]
     * [y] can be [var] [var = expr] [var [op] expr] [expr]
     * [z] can be [arithAssignExpr/inc/dec] (for now, any expr)
     */
    Token* keyword = consume();

    bool usingParens = expect(PAREN_OPEN);

    if (usingParens) consume();

    NodeRet setup = parse_for_setup();

    if (!expect_keyword(TO)) {
        assert(false);
    }

    consume(); // eat `to`

    NodeRet cond = parse_for_cond();

    bool hasDo = expect_keyword(DO);
    NodeRet change;
    if (hasDo) {
        consume(); // eat `do`

        change = parse_for_change();

        if (change.retCode != SUCCESS) {
            assert(false);
        }
    }

    NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        assert(false);
    }

    Node* forNode = create_parent_node(ST_FOR, keyword);

    vector_add(&forNode->children, setup.node);
    vector_add(&forNode->children, cond.node);
    if (hasDo) vector_add(&forNode->children, change.node);
    vector_add(&forNode->children, body.node);

    return (NodeRet){forNode, SUCCESS};
}

NodeRet parse_while_statement(void) {
    /*  while <expr> {
     *      ...
     *  }
     */
    Token* keyword = consume();

    ShuntRet expr = shunt(ptokens, t_pos, false);

    if (expr.err_code != SUCCESS) {
        assert(false);
    }

    t_pos = expr.tok_end_pos;

    if (!expect(CURLY_OPEN)) {
        assert(false);
    }

    NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        assert(false);
    }

    Node* whileNode = create_parent_node(ST_WHILE, keyword);

    vector_add(&whileNode->children, expr.expressionNode);
    vector_add(&whileNode->children, body.node);

    return (NodeRet){whileNode, SUCCESS};
}

NodeRet parse_foreach_statement(void) {
    /*  foreach <identifier> in <identifier> (with <identifier>)? {
     *    ...
     *  }
     */
    //[[todo]] add part for paren `(`
    Token* keyword = consume();

    if (!expect(IDENTIFIER)) {
        assert(false);
    }

    Token* element = consume();

    if (!expect_keyword(IN)) {
        assert(false);
    }

    consume(); // eat `in`

    if (!expect(IDENTIFIER)) {
        assert(false);
    }

    Token* array = consume();

    Node* foreachNode = create_parent_node(ST_FOREACH, keyword);

    vector_add(&foreachNode->children, create_leaf_node(TOKEN_WRAPPER, element));
    vector_add(&foreachNode->children, create_leaf_node(TOKEN_WRAPPER, array));

    if (expect_keyword(WITH)) {
        consume(); // eat `with`

        if (!expect(IDENTIFIER)) {
            assert(false);
        }

        Token* index = consume();

        vector_add(&foreachNode->children, create_leaf_node(TOKEN_WRAPPER, index));
    }

    if (!expect(CURLY_OPEN)) {
        assert(false);
    }

    NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        assert(false);
    }

    vector_add(&foreachNode->children, body.node);

    return (NodeRet){foreachNode, SUCCESS};
}

NodeRet parse_times_statement(void) {
    /*  times <expr> {
     *    ...
     *  }
     */

    Token* keyword = consume();

    ShuntRet expr = shunt(ptokens, t_pos, false);

    if (expr.err_code != SUCCESS) {
        assert(false);
    }

    t_pos = expr.tok_end_pos;

    if (!expect(CURLY_OPEN)) {
        assert(false);
    }

    NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        assert(false);
    }

    Node* timesNode = create_parent_node(ST_TIMES, keyword);

    vector_add(&timesNode->children, expr.expressionNode);
    vector_add(&timesNode->children, body.node);

    return (NodeRet){timesNode, SUCCESS};
}

NodeRet parse_ifelif_statement(bool isIf) {
    Token* keyword = current();
    consume(); // eat the `if` / `elif` //[[todo]] could be given to function?

    Node* ifNode = create_parent_node(isIf ? ST_IF : ST_ELIF, keyword);

    Token* next = peek();

    if (!next)
        assert(false);

    ShuntRet shuntData = shunt(ptokens, t_pos, false);

    if (shuntData.err_code != SUCCESS)
        assert(false);
    t_pos = shuntData.tok_end_pos;

    Node* conditionNode = shuntData.expressionNode;
    vector_add(&ifNode->children, conditionNode);

    Token* c = current();

    if (c->type != CURLY_OPEN)
        assert(false);

    NodeRet statement = parse_statement();

    if (statement.retCode != SUCCESS)
        assert(false);

    Node* statementBlockNode = statement.node;
    vector_add(&ifNode->children, statementBlockNode);

    return (NodeRet){ifNode, SUCCESS};
}

NodeRet parse_else_statement(void) {
    Token* keyword = consume(); // eat the `else`

    Node* elseNode = create_parent_node(ST_ELSE, keyword);

    if (!expect(CURLY_OPEN))
        assert(false);

    NodeRet statement = parse_statement();

    if (statement.retCode != SUCCESS)
        assert(false);

    Node* statementBlockNode = statement.node;
    vector_add(&elseNode->children, statementBlockNode);

    return (NodeRet){elseNode, SUCCESS};
}

NodeRet parse_if_statement(void) {
    /* whole If node structure
     *  |- if statement
     *  |- elif statements
     *  |  |- elif statement
     *  |  `- elif statement
     *  `- else statement
     */

    Node* node = create_parent_node(ST_IF_TOP_LEVEL, NULL);

    NodeRet ifNode = parse_ifelif_statement(true);

    if (ifNode.retCode != SUCCESS)
        assert(false);

    vector_add(&node->children, ifNode.node);

    Token* c;
    while (c = current(), c->type == KEYWORD && c->data.enum_pos == ELIF) {
        NodeRet elifNode = parse_ifelif_statement(false);

        if (elifNode.retCode != SUCCESS)
            assert(false);

        vector_add(&node->children, elifNode.node);
    }

    c = current();
    if (c->type == KEYWORD && c->data.enum_pos == ELSE) {
        NodeRet elseNode = parse_else_statement();

        if (elseNode.retCode != SUCCESS)
            assert(false);

        vector_add(&node->children, elseNode.node);
    }

    return (NodeRet){node, SUCCESS};
}

bool verify_token_for_print(Token* token) {
    return token && token->pos.start_line < plines->pos && token->pos.start_line == token->pos.end_line;
}

NodeRet parserr(const ParsErrors errorCode, Token* parent_token, Token* issue_token, ...) {
    switch (errorCode) {
        case PARSERR_BLOCK_MISSING_BRACE:
            puts(ATOM_CT__PARSERR_BLOCK_MISSING_BRACE);
            break;
        case PARSERR_SUB_CALL_PARSE_ERROR:
            puts(ATOM_CT__PARSERR_SUB_CALL_PARSE_ERROR);
            break;
        case PARSERR_SUB_CALL_EXPECTED_GOT_EXPR:
            puts(ATOM_CT__PARSERR_SUB_CALL_EXPECTED_GOT_EXPR);
            break;
        case PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT:
            puts(ATOM_CT__PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT);
            break;
        default:
            assert(false);
    }

    highlight_line_start_and_error(parent_token, issue_token, plines);

    return (NodeRet){NULL, errorCode};
}

NodeRet parse_un_op_statement(void) {
    ShuntRet expr = shunt(ptokens, t_pos, false);

    if (expr.err_code != SUCCESS) {
        assert(false);
    }
    t_pos = expr.tok_end_pos;

    // verify that an un op statement was parsed
    if (expr.expressionNode->type != EXPR_UN || ((Node*)expr.expressionNode->children.arr[0])->token->type != IDENTIFIER) { // [[todo]] more rigid error checking
        // the error here would probably just be expression statements aren't allowed
        assert(false);
    }

    return (NodeRet){expr.expressionNode, SUCCESS};
}

bool is_valid_index(int index) {
    return index >= 0 && index < ptokens->pos;
}

Token* confungry(int offset, bool consume, bool ignore_whitespace, bool ignore_newline) {
    // :(
}

/*          v-peek
 *  Tkn0    Tkn1    Tkn2
 *  ^-current       ^-peer(2)
 *  ^-consume
 *
 *  current/peek/peer means Tkn0 -> Tkn0 etc
 *  consume           means Tkn1 -> Tkn0, Tkn2 -> Tkn1  (Tkn1 becomes Tkn0)
 */
Token* peer(int amount) {
    if (!is_valid_index(t_pos + amount)) {
        return NULL;
    }

    return &ptokens->arr[t_pos + amount];
}

Token* peek(void) {
    if (!is_valid_index(t_pos + 1)) {
        return NULL;
    }

    return &ptokens->arr[t_pos + 1];
}

Token* justify(void) {
    if (!is_valid_index(t_pos - 1)) {
        return NULL;
    }

    return &ptokens->arr[t_pos - 1];
}

Token* consume(void) {
    if (!is_valid_index(t_pos)) {
        return NULL;
    }

    return &ptokens->arr[t_pos++];
}

Token* current(void) {
    if (!is_valid_index(t_pos)) {
        return NULL;
    }

    return &ptokens->arr[t_pos];
}

bool expect_op(TokenType type, ATOM_CT__LEX_OPERATORS_ENUM op) {
    return expect(type) && current()->data.enum_pos == op;
}

bool expect_keyword(ATOM_CT__LEX_KEYWORD_ENUM keyword) {
    return expect(KEYWORD) && current()->data.enum_pos == keyword;
}

bool expect(TokenType type) {
    Token* c = current();

    return c && c->type == type;
}
