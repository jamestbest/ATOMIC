//
// Created by jamescoward on 25/01/2024.
//

#include "ShuntingYard.h"

#include "Parserr.h"
#include "../Lexer/Lexer.h"

#include <SharedIncludes/Flag_shared.h>

static bool is_valid_index(ShuntData data, uint index);

static Token* current(ShuntData data);
static Token* peek(ShuntData data);
static Token* consume(ShuntData data);

static bool expect(ShuntData data, TokenType type);

static STATE get_next_state(STATE current_state, Token *current_t);

static bool c_is_valid_expr_cont(Token *current_t, STATE current_state);

static Node* form_operator_node(Token* op_token, Stack *output_s);

static void update_data(ShuntData data, ShuntRet ret);

static Node* parse_subroutine_call(ShuntData data);

//[[DEBUG]]
static void print_token_stack(const Stack* operator_stack);
static void print_node_stack(const Stack *output_s);

// function to determine if the current token is a valid continuation of the
//  expression given the current state
bool c_is_valid_expr_cont(Token *current_t, STATE current_state) {
    switch (current_state) {
        case EXPECTING_START:
        case EXPECTING_LEFT:
            return is_terminal(current_t) || is_l_paren(current_t) || current_t->type == EXPR_UN_PRE;
        case EXPECTING_CENTRE:
            return (is_arith_operator(current_t) && current_t->type != EXPR_UN_PRE) || is_r_paren(current_t) || is_square_bracket(current_t);
        default:
            assert(false);
    }
}

STATE get_next_state(STATE current_state, Token *current_t) {
    // assuming the current state was given a valid value
    switch (current_state) {
        case EXPECTING_START:
        case EXPECTING_LEFT:
            if (current_t->type == EXPR_UN_PRE || current_t->type == PAREN_OPEN) return EXPECTING_LEFT;
            return EXPECTING_CENTRE;
        case EXPECTING_CENTRE:
            if (current_t->type == EXPR_UN_POST || current_t->type == PAREN_CLOSE || current_t->type == BRACKET_CLOSE) return EXPECTING_CENTRE;
            return EXPECTING_LEFT;
    }
    assert(false);
}

ASS get_ass(ATOM_CT__LEX_OPERATORS_ENUM operator, TokenType token_type) {
    return OP_INFO[operator].assoc;
}

int get_pres(ATOM_CT__LEX_OPERATORS_ENUM operator, TokenType token_type) {
    //precedence goes from 0- , and a lower value means it will be lower in the tree
    return OP_INFO[operator].precedence;
}

int get_token_pres(Token* token) {
    return get_pres(token->data.enum_pos, token->type);
}

ASS get_token_ass(Token* token) {
    return get_ass(token->data.enum_pos, token->type);
}

ShuntRet parse_subroutine_call_arg(ShuntData data) {
    ShuntRet ret = shunt(data.tokens, *data.t_pos, true);
    update_data(data, ret);

    return ret;
}

Node* parse_subroutine_call_arguments(ShuntData data) {
    Node* argsNode = create_parent_node(EXPRESSION, SUB_CALL_ARGS,NULL);

    Token* c;
    if (c = current(data), !c || c->type == PAREN_CLOSE) {
        return argsNode;
    }

    bool first_arg = true;
    do {
        if (first_arg) first_arg = false;
        else consume(data); // eat the comma

        ShuntRet arg = parse_subroutine_call_arg(data);

        if (arg.err_code != SUCCESS) assert(false);

        vector_add(&argsNode->children, arg.expressionNode);
    } while(c = current(data), c && c->type == COMMA);

    return argsNode;
}

Node* parse_subroutine_call(ShuntData data) {
    Node* funcNode = create_parent_node(EXPRESSION, SUB_CALL,NULL);

    Token* func_name = consume(data);
    consume(data); //eat the (

    Node* args = parse_subroutine_call_arguments(data);

    if (!expect(data, PAREN_CLOSE)) {
        assert(false);
    }

    consume(data); // eat the )

    vector_add(&funcNode->children, create_leaf_node(EXPRESSION, TOKEN_WRAPPER, func_name));
    vector_add(&funcNode->children, args);

    return funcNode;
}

void parse_identifier(ShuntData data, Token* next, Stack* output_s) {
    /*  Could be an identifier or a function call
     *      - Identifier: push to the output q
     *      - Function call: need to shunt the arguments
     */

    if (next->type == PAREN_OPEN) {
        stack_push(output_s, parse_subroutine_call(data));
    } else {
        stack_push(output_s, create_leaf_node(EXPRESSION, EX_LIT, consume(data)));
    }
}

ShuntRet shunt(const tokenArray* tokens, uint t_pos, bool ignoreTrailingParens) {
    Stack output_s = stack_create(MIN_QUEUE_SIZE);
    Stack operator_s = stack_create(MIN_STACK_SIZE);

    const ShuntData data = (ShuntData){tokens, &t_pos};

    STATE c_state = EXPECTING_START;

    const bool expr_dbg = flag_get_value(ATOM_CT__FLAG_EXPR_DBG);
    const bool verbose_expr_dbg = flag_get_value(ATOM_CT__FLAG_VEXPR_DBG);

    Token* c;
    while (c = current(data), c_is_valid_expr_cont(c, c_state)) {
        if (verbose_expr_dbg) {
            print_token_stack(&operator_s);
            print_node_stack(&output_s);
        }

        switch (c->type) {
            case LIT_INT:
            case LIT_FLOAT:
            case LIT_STR:
            case LIT_BOOL:
            case LIT_CHR:
            case LIT_NAV:
            case TYPE:
                stack_push(&output_s, create_leaf_node(EXPRESSION, EX_LIT, consume(data)));
                break;
            case IDENTIFIER:
                parse_identifier(data, peek(data), &output_s);
                break;
            case EXPR_BIN:
            case EXPR_TRINARY:
            case EXPR_UN_PRE:
            case EXPR_UN_POST: {
                Token* o2;
                consume(data);
                while (o2 = stack_peek(&operator_s), o2 && !(o2->type == PAREN_OPEN || o2->type == BRACKET_OPEN) &&
                        (get_token_pres(o2) < get_token_pres(c) ||
                        (get_token_pres(o2) == get_token_pres(c) && get_token_ass(c) == LEFT))
                      ) {
                    Node* op_node = form_operator_node(stack_pop(&operator_s), &output_s);
                    stack_push(&output_s, op_node);
                }
                stack_push(&operator_s, c);
                break;
            }
            case PAREN_OPEN:
                stack_push(&operator_s, consume(data));
                break;
            case PAREN_CLOSE: {
                Token* o1;

                while (o1 = stack_peek(&operator_s), o1 && o1->type != PAREN_OPEN) {
                    Node* op_node = form_operator_node(stack_pop(&operator_s), &output_s);
                    stack_push(&output_s, op_node);
                }

                if (o1 = stack_peek(&operator_s), !o1 || o1->type != PAREN_OPEN) { //[[maybe]] is the && correct here?
                    if (!ignoreTrailingParens)
                        assert(false); //[[todo]] mismatch
                    goto shunt_end_of_while; // we don't want to continue shunting, there is no more parsing to do
                }

                consume(data);
                stack_pop(&operator_s);
                break;
            }
            case BRACKET_OPEN:
                stack_push(&operator_s, consume(data));
                break;
            case BRACKET_CLOSE: {
                /*  This is <var>[<expr>]
                 *      Once we reach a close bracket we just need to:
                 *          1. check if the top of the operator stack is a BRACKET_OPEN
                 *          2. check that there is something in the output stack
                 *          3. if there isn't then throw an error for empty bracket expression
                 *          4. else add it to the Node and pop the BRACKET_OPEN, add result to output
                 */
                Token* o1;
                while (o1 = stack_peek(&operator_s), o1 && o1->type != BRACKET_OPEN) {
                    Node* op_node = form_operator_node(stack_pop(&operator_s), &output_s);
                    stack_push(&output_s, op_node);
                }

                if (o1 = stack_peek(&operator_s), !o1 || o1->type != BRACKET_OPEN) { //[[maybe]] is the && correct here?
                    if (!ignoreTrailingParens)
                        assert(false);
                }

                Token* l_bracket = stack_peek(&operator_s);

                if (!l_bracket || l_bracket->type != BRACKET_OPEN) {
                    assert(false);
                }

                Node* out = stack_peek(&output_s);

                if (!out) {
                    assert(false);
                }

                // todo: should this still be an expr binary??? or should this start to specialise at this point!?
                Node* arrayNode = create_parent_node(EXPRESSION, EXPR_BINARY, stack_pop(&operator_s));

                Node* expr = stack_pop(&output_s);
                Node* identifier = stack_pop(&output_s);

                if (!identifier) {
                    assert(false);
                }

                if (identifier->type != EX_LIT || identifier->token->type != IDENTIFIER) {
                    assert(false);
                }

                vector_add(&arrayNode->children, identifier);
                vector_add(&arrayNode->children, expr);

                stack_push(&output_s, arrayNode);

                consume(data); // eat the ']'
            }
        }
        c_state = get_next_state(c_state, c);
    }

shunt_end_of_while:
    if (verbose_expr_dbg) {
        print_token_stack(&operator_s);
        print_node_stack(&output_s);
    }

    while (!stack_empty(&operator_s)) {
        Token* o1 = stack_peek(&operator_s);
        if (o1->type == PAREN_OPEN) {
            assert(false); //[[todo]] error
        }

        Node* expr_node = form_operator_node(stack_pop(&operator_s), &output_s);
        stack_push(&output_s,expr_node);
    }

    Node* ret = stack_pop(&output_s);

    if (expr_dbg) {
        puts("END EXPR: ");
        print_top_level_node(ret);
        putchar('\n');
    }

    stack_destroy(&output_s);
    stack_destroy(&operator_s);

    return (ShuntRet){ret, t_pos, SUCCESS};
}

Node* form_un_op_node(Token* op_token, Stack* output_s) {
    Node* child = stack_pop(output_s);

    if (!child) {
        assert(false); //[[todo]] errors
    }

    Node* op_node = create_parent_node(EXPRESSION, EXPR_UNARY, op_token);

    vector_add(&op_node->children, child);

    return op_node;
}

Node* form_bin_op_node(Token* op_token, Stack* output_s) {
    Node* r_expr = stack_pop(output_s);
    Node* l_expr = stack_pop(output_s);

    if (!l_expr || !r_expr) {
        assert(false); //[[todo]] add errors to shunting yard
    }

    NodeType type = op_token->type == EXPR_BIN ? EXPR_BINARY : EXPR_ASSIGNMENT;

    Node* op_node = create_parent_node(EXPRESSION, type, op_token);

    vector_add(&op_node->children, l_expr);
    vector_add(&op_node->children, r_expr);

    return op_node;
}

Node* form_tri_op_node(Token* op_token, Stack* output_s) {
    assert(false);
}

Node* form_operator_node(Token* op_token, Stack *output_s) {
    switch (op_token->type) {
        case EXPR_UN_PRE:
        case EXPR_UN_POST:
            return form_un_op_node(op_token, output_s);
        case EXPR_BIN:
            return form_bin_op_node(op_token, output_s);
        case EXPR_TRINARY:
            return form_tri_op_node(op_token, output_s);
    }
    assert(false);
}

bool is_valid_index(ShuntData data, uint index) {
    return index < data.tokens->pos;
}

Token* current(ShuntData data) {
    if (!is_valid_index(data, *data.t_pos)) {
        return NULL;
    }

    return token_arr_ptr(data.tokens, *data.t_pos);
}

Token* peek(ShuntData data) {
    if (!is_valid_index(data, (*data.t_pos) + 1)) {
        return NULL;
    }

    return token_arr_ptr(data.tokens, *data.t_pos + 1);
}

Token* consume(ShuntData data) {
    if (!is_valid_index(data, *data.t_pos)) {
        return NULL;
    }

    return token_arr_ptr(data.tokens, (*data.t_pos)++);
}

bool expect(ShuntData data, TokenType type) {
    Token* c = current(data);

    if (!c) return false;

    return c->type == type;
}

void update_data(ShuntData data, ShuntRet ret) {
    *data.t_pos = ret.tok_end_pos;
}

//[[DEBUG]]
void print_token_stack(const Stack* operator_stack) {
    printf("---------------STACK OP TOKENS--------------- \n");

    if (operator_stack->ptr == 0) {
        printf("\tNONE\n");
        return;
    }

    for (uint i = 0; i < operator_stack->ptr; ++i) {
        Token* tok = operator_stack->arr[i];
        printf("[%d]: ", i);
        print_token_ln(tok);
        printf("\n");
    }
}

void print_node_stack(const Stack *output_s) {
    printf("---------------QUEUE OUTPUT NODES--------------\n");

    if (output_s->ptr == 0) {
        printf("\tNONE\n");
        return;
    }

    for (uint i = 0; i < output_s->ptr; ++i) {
        Node* node = output_s->arr[i];
        print_top_level_node(node);
        printf("\n");
    }
}
