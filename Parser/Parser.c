//
// Created by jamescoward on 21/01/2024.
//

#include "Parser.h"

const Array* ptokens;
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
static NodeRet RETIRED_parse_unary_statement(void);

static NodeRet parse_subroutine_call(void);

static NodeRet RETIRED_parse_un_op_statement(void);

static NodeRet parse_expression_statement(void);

static Node* construct_assignment_node(Node* lvalue, Node* operator, Node* rvalue);

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

// [[todo]] I've just realised that the Node* created are leaked if there are error! --yes I should have realised this earlier


void propagate_stmt_uid(Node* node, uint64_t stmt_uid) {
    if (!node->children.arr) return;

    for (uint i = 0; i < node->children.pos; ++i) {
        Node* child = node->children.arr[i];

        if (child->statement_id != -1) propagate_stmt_uid(child, child->statement_id);
        else {
            child->statement_id = stmt_uid;
            propagate_stmt_uid(child, stmt_uid);
        }
    }
}

void _add_node_to_children(Node* parentNode, Node* node, bool is_stmt) {
    if (!parentNode || !node) {
        printf(C_RED"DEVERR: <<NULL>> parent (%p) or statement (%p) in add statement"C_RST, parentNode, node);
        return;
    }

    if (node->type != NODE_MULTIPLE_STATEMENTS) {
        if (is_stmt) {
            propagate_stmt_uid(node, node->statement_id);
        }
        vector_add(&parentNode->children, node);
        return;
    }

    for (uint i = 0; i < node->children.pos; ++i) {
        Node* stmt = node->children.arr[i];
        if (is_stmt) {
            propagate_stmt_uid(stmt, stmt->statement_id);
        }
        vector_add(&parentNode->children, stmt);
    }
    free_node_head(node);
}

void add_statement_to_children(Node* parentNode, Node* statement) {
    _add_node_to_children(parentNode, statement, true);
}

void add_statement(Node* statement) {
    add_statement_to_children(file_global_node, statement);
}

void add_node_to_children(Node* parentNode, Node* node) {
    _add_node_to_children(parentNode, node, false);
}

NodeRet parse(const Array* tokens, const Vector* lines) {
    ptokens = tokens;
    plines = lines;
    t_pos = 0;

    uint ret_code = SUCCESS;

    Token* c;

    file_global_node = create_parent_node(NODEGT_ROOT, NODE_ROOT,NULL);

    while (c = current(), c != NULL) {
        const NodeRet ret = parse_statement();

        if (ret.retCode == END) {
            break;
        }

        if (ret.retCode != SUCCESS) {
            ret_code = ret.retCode;
        }

        add_statement(ret.node);
    }

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

bool has_valid_statement_starter(void) {
    Token* c = current();

    if (!c) return false;

    switch (c->type) {
        case IDENTIFIER:
        case KEYWORD:
        case CURLY_OPEN:
        case OP_UN_PRE:
            return true;

        default:
            return false;
    }
}

Node* construct_assignment_node(Node* lvalue, Node* operator, Node* rvalue) {
    Node* assigment = create_parent_node(STATEMENT, ST_VAR_ASS, NULL);

    add_node_to_children(assigment, lvalue);
    add_node_to_children(assigment, operator);
    add_node_to_children(assigment, rvalue);

    return assigment;
}

NodeRet parse_statement(void) {
    //[[todo]] could be a single statement or a block ('{')
    // this will help with having if (expr) statement
    // without braces
    NodeRet ret;
    Token* t;

    skip_all_skippables();

    if (!has_valid_statement_starter()) {
        return (NodeRet){NULL, END};
    }

    t = current();

    switch (t->type) {
        case OP_UN_PRE:
            ret = parse_expression_statement();
            break;

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
            parserr(PARSERR_UNEXPECTED_TOKEN_STATEMENT_START, NULL, t);
            consume();
            ret = construct_error_node(t);
            break;
    }

    skip_all_skippables();

    return ret;
}

NodeRet parse_subroutine_call_modifier(void) {
    Token* modifier = current();

    //todo
}

NodeRet parse_variable_declaration_modifier(void) {

}

NodeRet parse_subroutine_call(void) {
    Token* identifier = current();

    const ShuntRet sub_call = shunt(ptokens, t_pos, false);

    if (sub_call.err_code != SUCCESS) {
        return parserr(PARSERR_SUB_CALL_PARSE_ERROR, current(), NULL);
    }

    if (sub_call.expressionNode->type != SUB_CALL) {
        return parserr(PARSERR_SUB_CALL_EXPECTED_GOT_EXPR, identifier, NULL);
    }

    t_pos = sub_call.tok_end_pos;

    Node* stmt_expr = create_parent_node(STATEMENT, ST_EXPR, NULL);
    add_node_to_children(stmt_expr, sub_call.expressionNode);

    return (NodeRet){stmt_expr, SUCCESS};
}

NodeRet parse_assignment(void) {
    /* This could be from
     *  a = 12
     *  a += 12
     *  THIS WILL BE HANDLED SEPARATELY b::i4 = 12 + a  --implicit cast
     */
    Token* identifier = current();

    // first parse the lvalue
    const ShuntRet lvalue = shunt(ptokens, t_pos, false);

    if (lvalue.err_code != SUCCESS)
        assert(false);
    t_pos = lvalue.tok_end_pos;

    if (!expect(OP_ASSIGN) && !expect(OP_ARITH_ASSIGN)) {
        assert(false);
    }

    Token* assign_op = consume();

    const ShuntRet rvalue = shunt(ptokens, t_pos, false);

    if (rvalue.err_code != SUCCESS) {
        assert(false);
    }
    t_pos = rvalue.tok_end_pos;

    Node* operator = create_leaf_node(EXPRESSION, TOKEN_WRAPPER, assign_op);
    Node* var_assign = construct_assignment_node(lvalue.expressionNode, operator, rvalue.expressionNode);

    return (NodeRet){var_assign, SUCCESS};
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

    Node* declNode = create_parent_node(STATEMENT, ST_VAR_DECL, type_op);

    vector_add(&declNode->children, create_leaf_node(DECLARATION, TOKEN_WRAPPER, identifier));
    vector_add(&declNode->children, create_leaf_node(DECLARATION, TOKEN_WRAPPER, type));

    if (!expect(OP_ASSIGN)) {
        return (NodeRet){declNode, SUCCESS};
    }

    consume(); // eat the `=`

    const ShuntRet exprInfo = shunt(ptokens, t_pos, false);

    if (exprInfo.err_code != SUCCESS)
        assert(false);
    t_pos = exprInfo.tok_end_pos;

    vector_add(&declNode->children, exprInfo.expressionNode);

    return (NodeRet){declNode, SUCCESS};
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
        case OP_ASSIGN:
        case OP_ARITH_ASSIGN:
        case BRACKET_OPEN:
            return parse_assignment();
        case OP_UN_POST:
            return parse_expression_statement();
        default:
            consume(); // eat the error identifier
            return parserr(PARSERR_UNEXPECTED_TOKEN_IDENTIFIER_STATEMENT, identifier, next);
    }
}

/*  This could be:
 *      ++var
 *      *(ptr)
 *      *(ptr) = <>
 */
NodeRet RETIRED_parse_unary_statement(void) {
    return parse_expression_statement();
}

uint extract_error_code(uint errCode) {
    return errCode == SUCCESS || errCode == END ? SUCCESS : errCode;
}

NodeRet parse_statement_block(void) {
    Token* token = current();

    Node* parent_node = create_parent_node(STATEMENT, ST_BLOCK, token);

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

    child = parse_statement();
    while (child.retCode == SUCCESS) {
        add_statement_to_children(parent_node, child.node);

        if (current() && current()->type == CURLY_CLOSE)
            break;

        child = parse_statement();
    }

    if (!current() || current()->type != CURLY_CLOSE) {
        return parserr(PARSERR_BLOCK_MISSING_BRACE, parent_node->token, current());
    }

    consume(); // eat the CURLY_CLOSE

    uint retErr = extract_error_code(child.retCode);

    return (NodeRet){parent_node, retErr};
}

NodeRet parse_keyword(void) {
    const Token* t = current();

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
     * var :: type = expr       --IMPLICIT CAST + DECL + OP_ASSIGN
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
     * var :: type = expr       --IMPLICIT CAST + DECL + OP_ASSIGN
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

    Node* paramNode = create_parent_node(DECLARATION, SUB_PARAM, identifier);

    vector_add(&paramNode->children, create_leaf_node(DECLARATION, TOKEN_WRAPPER, identifier));
    vector_add(&paramNode->children, create_leaf_node(DECLARATION, TOKEN_WRAPPER, type));

    return (NodeRet){paramNode, SUCCESS};
}

NodeRet parse_params(void) {
    if (!expect(PAREN_OPEN)) {
        assert(false);
    }

    consume(); // eat `(`

    Node* params = create_parent_node(DECLARATION, SUB_PARAMS,NULL);

    while (expect(IDENTIFIER)) {
        const NodeRet param = parse_param();

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

NodeRet parse_subroutine_statement(bool is_func) {
    Token* keyword = consume();

    Node* sub_node = create_parent_node(STATEMENT, is_func ? ST_FUNC : ST_PROC, keyword);

    //[[todo]] for now just assuming there are no keyword modifiers

    if (!expect(IDENTIFIER)) {
        assert(false);
    }

    Token* sub_name = consume();

    const NodeRet params = parse_params();

    if (params.retCode != SUCCESS) {
        assert(false);
    }

    Token* type;
    if (is_func) {
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

    const NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        parserr(PARSERR_SUB_STATEMENT_ERROR_IN_BODY, keyword, current());
    }

    vector_add(&sub_node->children, create_leaf_node(DECLARATION, TOKEN_WRAPPER, sub_name));
    vector_add(&sub_node->children, params.node);
    if (is_func) vector_add(&sub_node->children, create_leaf_node(DECLARATION, TOKEN_WRAPPER, type));
    vector_add(&sub_node->children, body.node);

    return (NodeRet){sub_node, SUCCESS};
}

NodeRet parse_func_statement(void) {
    return parse_subroutine_statement(true);
}

NodeRet parse_proc_statement(void) {
    return parse_subroutine_statement(false);
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

    Node* contNode = create_parent_node(STATEMENT, ST_CONT, consume());

    return (NodeRet){contNode, SUCCESS};
}

NodeRet parse_break_statement(void) {
    // for now the break statement will just be a keyword
    // but I would imagine that having labels / a way to continue a different loop

    Node* brkNode = create_parent_node(STATEMENT, ST_BRK, consume());

    return (NodeRet){brkNode, SUCCESS};
}

NodeRet parse_ret_statement(void) {
    Token* t = consume(); //eat the ret keyword

    Node* retNode = create_parent_node(STATEMENT, ST_RET, t);

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
    //  can be a statement or a statement chain

    /*  Valid statements
     *   - variable declarations    - i: i4 = 12
     *   - variable assignments     - i = 10
     */

    if (!expect(IDENTIFIER)) {
        assert(false);
    }

    Node* stmt_chain = create_parent_node(STATEMENT, ST_CHAIN,NULL);

    while (true) {
        if (expect_keyword(TO)) break;

        const Token* n = peek();

        if (!n) assert(false);

        if (n->type == TYPE_SET || n->type == TYPE_IMPL_CAST) {
            const NodeRet setup = parse_var_declaration();

            if (setup.retCode != SUCCESS) assert(false);

            add_statement_to_children(stmt_chain, setup.node);
        } else if (n->type == OP_ASSIGN || n->type == OP_ARITH_ASSIGN) {
            const NodeRet assignment = parse_assignment();

            if (assignment.retCode != SUCCESS) assert(false);

            add_statement_to_children(stmt_chain, assignment.node);
        } else {
            assert(false);
        }

        if (!expect(DELIMITER)) break;

        skip_all_skippables();
    }

    if (stmt_chain->children.pos == 1) {
        const NodeRet ret = (NodeRet){stmt_chain->children.arr[0], SUCCESS};
        free_node_head(stmt_chain);

        return ret;
    }

    return (NodeRet){stmt_chain, SUCCESS};
}

NodeRet parse_for_cond(void) {
    //* An expression (that evaluates to a boolean (Not the job of this parse phase))

    ShuntRet expr = shunt(ptokens, t_pos, true); // we want to ignore trailing because `do` is optional and this could be in parens

    if (expr.err_code != SUCCESS) {
        assert(false);
    }

    t_pos = expr.tok_end_pos;

    return (NodeRet){expr.expressionNode, SUCCESS};
}

NodeRet parse_for_change(void) {
    const ShuntRet expr = shunt(ptokens, t_pos, true);

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
     * [x] is a list of statments seperated by `;`
     * [y] is an expression (that evaluates to a boolean)
     * [z] is an expression
     *
     * All are optional
     */
    Token* keyword = consume();

    const bool usingParens = expect(PAREN_OPEN);

    if (usingParens) consume();

    const NodeRet setup = parse_for_setup();

    if (!expect_keyword(TO)) {
        assert(false);
    }

    consume(); // eat `to`

    const NodeRet cond = parse_for_cond();

    const bool hasDo = expect_keyword(DO);
    NodeRet change;
    if (hasDo) {
        consume(); // eat `do`

        change = parse_for_change();

        if (change.retCode != SUCCESS) {
            assert(false);
        }
    }

    const NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        assert(false);
    }

    Node* forNode = create_parent_node(STATEMENT, ST_FOR, keyword);

    add_statement_to_children(forNode, setup.node);
    add_node_to_children(forNode, cond.node);
    if (hasDo) add_node_to_children(forNode, change.node);

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

    Node* whileNode = create_parent_node(STATEMENT, ST_WHILE, keyword);

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

    Node* foreachNode = create_parent_node(STATEMENT, ST_FOREACH, keyword);

    vector_add(&foreachNode->children, create_leaf_node(EXPRESSION, TOKEN_WRAPPER, element));
    vector_add(&foreachNode->children, create_leaf_node(EXPRESSION, TOKEN_WRAPPER, array));

    if (expect_keyword(WITH)) {
        consume(); // eat `with`

        if (!expect(IDENTIFIER)) {
            assert(false);
        }

        Token* index = consume();

        vector_add(&foreachNode->children, create_leaf_node(EXPRESSION, TOKEN_WRAPPER, index));
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

    // todo: this does not allow for single statements following if/for/etc
    if (!expect(CURLY_OPEN)) {
        assert(false);
    }

    NodeRet body = parse_statement_block();

    if (body.retCode != SUCCESS) {
        assert(false);
    }

    Node* timesNode = create_parent_node(STATEMENT, ST_TIMES, keyword);

    vector_add(&timesNode->children, expr.expressionNode);
    vector_add(&timesNode->children, body.node);

    return (NodeRet){timesNode, SUCCESS};
}

NodeRet parse_ifelif_statement(bool isIf) {
    Token* keyword = current();
    consume(); // eat the `if` / `elif` //[[todo]] could be given to function?

    Node* ifNode = create_parent_node(STATEMENT, isIf ? ST_IF : ST_ELIF, keyword);

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

    NodeRet statement = parse_statement_block();

    if (statement.retCode != SUCCESS)
        assert(false);

    Node* statementBlockNode = statement.node;
    vector_add(&ifNode->children, statementBlockNode);

    return (NodeRet){ifNode, SUCCESS};
}

NodeRet parse_else_statement(void) {
    Token* keyword = consume(); // eat the `else`

    Node* elseNode = create_parent_node(STATEMENT, ST_ELSE, keyword);

    if (!expect(CURLY_OPEN))
        assert(false);

    NodeRet statement = parse_statement_block();

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

    Node* node = create_parent_node(STATEMENT, ST_IF_TOP_LEVEL,NULL);

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

/*
 *  This could be just an expression statement that has change e.g. a++
 *  This could also be an assignment expression e.g. *a = 12
 */
NodeRet parse_expression_statement(void) {
    const ShuntRet lvalue = shunt(ptokens, t_pos, false);

    if (lvalue.err_code != SUCCESS) {
        assert(false);
    }
    t_pos = lvalue.tok_end_pos;

    if (!expect(OP_ASSIGN) && !expect(OP_ARITH_ASSIGN)) {
        Node* exprStatement = create_parent_node(STATEMENT, ST_EXPR,NULL);

        vector_add(&exprStatement->children, lvalue.expressionNode);

        return (NodeRet){exprStatement, SUCCESS};
    }

    Token* assign_op = consume();

    const ShuntRet rvalue = shunt(ptokens, t_pos, false);

    if (rvalue.err_code != SUCCESS) {
        assert(false);
    }
    t_pos = rvalue.tok_end_pos;

    Node* operator = create_leaf_node(EXPRESSION, TOKEN_WRAPPER, assign_op);
    Node* assignment = construct_assignment_node(lvalue.expressionNode, operator, rvalue.expressionNode);

    return (NodeRet){assignment, SUCCESS};
}


/*  This can be assignment or inc/dec
 *  *(a+100) = 12           TODO:: NEED TO HANDLE THIS CASE
 *  ++b
 *  --c
 */
NodeRet RETIRED_parse_un_op_statement(void) {
    const ShuntRet expr = shunt(ptokens, t_pos, false);

    if (expr.err_code != SUCCESS) {
        assert(false);
    }
    t_pos = expr.tok_end_pos;

    // verify that an un op statement was parsed
    if (expr.expressionNode->type != EXPR_UN || ((Node*)expr.expressionNode->children.arr[0])->token->type != IDENTIFIER) { // [[todo]] more rigid error checking
        // the error here would probably just be expression statements aren't allowed
        assert(false);
    }

    if (!expect(OP_ASSIGN) && !expect(OP_ARITH_ASSIGN)) {
        return (NodeRet){expr.expressionNode, SUCCESS};
    }
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

    return arr_get(ptokens, t_pos + amount);
}

Token* peek(void) {
    if (!is_valid_index(t_pos + 1)) {
        return NULL;
    }

    return arr_get(ptokens, t_pos + 1);
}

Token* justify(void) {
    if (!is_valid_index(t_pos - 1)) {
        return NULL;
    }

    return arr_get(ptokens, t_pos - 1);
}

// todo: need to allow `\` to mean continue statement to next line
Token* consume(void) {
    if (!is_valid_index(t_pos)) {
        return NULL;
    }

    return arr_get(ptokens, t_pos++);
}

Token* current(void) {
    if (!is_valid_index(t_pos)) {
        return NULL;
    }

    return arr_get(ptokens, t_pos);
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
