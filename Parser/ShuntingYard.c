//
// Created by jamescoward on 25/01/2024.
//

#include "ShuntingYard.h"

static Token* current(ShuntData data);
static Token* peek(ShuntData data);
static Token* consume(ShuntData data);

static STATE get_next_state(STATE current_state, Token *current_t);

static bool c_is_valid_expr_cont(Token *current_t, STATE current_state);

static Node* form_operator_node(Token* op_token, Stack *output_s);

static void update_data(ShuntData data, ShuntRet ret);

//[[DEBUG]]
static void print_token_stack(Stack* operator_stack);
static void print_node_stack(Stack *output_s);

// function to determine if the current token is a valid continuation of the
//  expression given the current state
bool c_is_valid_expr_cont(Token *current_t, STATE current_state) {
    switch (current_state) {
        case EXPECTING_START:
        case EXPECTING_LEFT:
            return is_terminal(current_t) || is_l_paren(current_t) || current_t->type == OP_UN_PRE;
        case EXPECTING_CENTRE:
            return (is_operator(current_t) && current_t->type != OP_UN_PRE) || is_r_paren(current_t);
        default:
            assert(false);
    }
}

STATE get_next_state(STATE current_state, Token *current_t) {
    // assuming the current state was given a valid value
    switch (current_state) {
        case EXPECTING_START:
        case EXPECTING_LEFT:
            if (current_t->type == OP_UN_PRE || current_t->type == PAREN_OPEN) return EXPECTING_LEFT;
            return EXPECTING_CENTRE;
        case EXPECTING_CENTRE:
            if (current_t->type == PAREN_CLOSE) return EXPECTING_CENTRE;
            return EXPECTING_LEFT;
    }
}

ASS get_ass(ATOM_CT__LEX_OPERATORS_ENUM operator, TokenType token_type) {
    switch (operator) {
        case PLUS:
        case MINUS:
        case MULT:
        case DIV:
        case MOD:
            return LEFT;

        case POW:
            return RIGHT;

        case BAND:
        case BOR:
        case SHL:
        case SHR:
            return LEFT;

        case ASS_PLUS:
        case ASS_MINUS:
        case ASS_MULT:
        case ASS_DIV:
        case ASS_MOD:
        case ASS_POW:
        case ASS_BAND:
        case ASS_BOR:
        case ASS_SHL:
        case ASS_SHR:
            return RIGHT;

        case LAND:
        case LOR:
        case LXOR:
            return LEFT;


        case LNOT:
            return RIGHT;

        case BXOR:
        case BNOT:
            return LEFT;

        case INC: //[[todo]] in C the post and pre inc have different ASS
        case DEC:
            return LEFT;

        case QUESTION:
            return RIGHT;

        case AMPERSAND:
            return RIGHT;

        case ASSIGNMENT:
            return RIGHT;

        case EQU:
        case NEQ:
            return LEFT;

        case LESS:
        case MORE:
        case LESSEQ:
        case MOREEQ:
            return LEFT;

        case SWAP:
        case RANGE:
            return LEFT; //[[todo]] check

        case ARROW:
            return LEFT;

        case DEREFERENCE:
            return RIGHT;
    }
}

int get_pres(ATOM_CT__LEX_OPERATORS_ENUM operator, TokenType token_type) {
    //precedence goes from 0- , and a lower value means it will be lower in the tree
    switch (operator) {
        case ARROW:
            return 0;

        case INC:
        case DEC:
            return (token_type == OP_UN_PRE);

        case MINUS:
        case PLUS:
            if (token_type == OP_UN_PRE) return 1; //unary +/- e.g. -a
            return 3; // binary operator e.g. a + b

        case POW:
        case LNOT:
        case BNOT:
        case AMPERSAND:
        case DEREFERENCE:
        case SWAP: // [[maybe]] !sure where to place swap
            return 1;

        case MULT:
        case DIV:
        case MOD:
            return 2;

        case SHL:
        case SHR:
            return 4;

        case LESS:
        case MORE:
        case LESSEQ:
        case MOREEQ:
            return 5;

        case EQU:
        case NEQ:
            return 6;

        case BAND:
            return 7;

        case BXOR:
            return 8;

        case BOR:
            return 9;

        case LAND:
            return 10;

        case LXOR:
            return 11;

        case LOR:
            return 12;

        case QUESTION:
            return 13;

        case RANGE:
            return 14;

        case ASS_PLUS:
        case ASS_MINUS:
        case ASS_MULT:
        case ASS_DIV:
        case ASS_MOD:
        case ASS_POW:
        case ASS_BAND:
        case ASS_BOR:
        case ASS_SHL:
        case ASS_SHR:
        case ASSIGNMENT:
            return 15;
    }
}

int get_token_pres(Token* token) {
    return get_pres(token->data.enum_pos, token->type);
}

ASS get_token_ass(Token* token) {
    return get_ass(token->data.enum_pos, token->type);
}

Node* parse_function_call(ShuntData data) {
    Node* funcNode = create_parent_node(FUNC_CALL, NULL);

    Token* func_name = consume(data);

    //[[todo]] add multi param
    ShuntRet ret = shunt(data.tokens, *data.t_pos, true);
    update_data(data, ret);

    if (ret.err_code != SUCCESS) assert(false);

    vector_add(&funcNode->children, create_leaf_node(TOKEN_WRAPPER, func_name));
    vector_add(&funcNode->children, ret.expressionNode);

    return funcNode;
}

void parse_identifier(ShuntData data, Token* current, Token* next, Stack* output_s) {
    /*  Could be an identifier or a function call
     *      - Identifier: push to the output q
     *      - Function call: need to shunt the arguments
     */

    if (next->type == PAREN_OPEN) {
        stack_push(output_s, parse_function_call(data));
    } else {
        stack_push(output_s, create_leaf_node(EX_LIT, current));
    }
}

ShuntRet shunt(const Token_vec *tokens, uint t_pos, bool ignoreTrailingParens) {
    Stack output_s = stack_create(MIN_QUEUE_SIZE);
    Stack operator_s = stack_create(MIN_STACK_SIZE);

    ShuntData data = (ShuntData){tokens, &t_pos};

    STATE c_state = EXPECTING_START;

    Token* c;
    while (c = current(data), c_is_valid_expr_cont(c, c_state)) {
        print_token_stack(&operator_s);
        print_node_stack(&output_s);

        switch (c->type) {
            case LIT_INT:
                stack_push(&output_s, create_leaf_node(EX_LIT, c));
                break;
            case IDENTIFIER:
                parse_identifier(data, c, peek(data), &output_s);
                break;
            case OP_BIN:
            case OP_BIN_OR_UN:
            case OP_TRINARY:
            case OP_UN:
            case OP_UN_PRE:
            case OP_UN_POST: {
                Token* o2;
                while (o2 = stack_peek(&operator_s), o2 && o2->type != PAREN_OPEN &&
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
                stack_push(&operator_s, c);
                break;
            case PAREN_CLOSE: {
                Token* o1;

                while (o1 = stack_peek(&operator_s), o1 && o1->type != PAREN_OPEN) {
                    Node* op_node = form_operator_node(stack_pop(&operator_s), &output_s);
                    stack_push(&output_s, op_node);
                }

                if (o1 = stack_peek(&operator_s), o1 && o1->type != PAREN_OPEN) { //[[maybe]] is the && correct here?
                    assert(false); //[[todo]] mismatch
                }

                stack_pop(&operator_s);
                break;
            }
        }
        c_state = get_next_state(c_state, c);
        consume(data);
    }

    print_token_stack(&operator_s);
    print_node_stack(&output_s);

    while (!stack_empty(&operator_s)) {
        Token* o1 = stack_peek(&operator_s);
        if (o1->type == PAREN_OPEN) {
            assert(false); //[[todo]] error
        }

        Node* expr_node = form_operator_node(stack_pop(&operator_s), &output_s);
        stack_push(&output_s,expr_node);
    }

    Node* ret = (Node*)stack_pop(&output_s);

    puts("END EXPR: ");
    print_top_level_node(ret);

    stack_destroy(&output_s);
    stack_destroy(&operator_s);

    return (ShuntRet){ret, t_pos, SUCCESS};
}

Node* form_un_op_node(Token* op_token, Stack* output_s) {
    Node* child = stack_pop(output_s);

    if (!child) {
        assert(false); //[[todo]] errors
    }

    Node* op_node = create_parent_node(EXPR, op_token);

    vector_add(&op_node->children, child);

    return op_node;
}

Node* form_bin_op_node(Token* op_token, Stack* output_s) {
    Node* r_expr = stack_pop(output_s);
    Node* l_expr = stack_pop(output_s);

    if (!l_expr || !r_expr) {
        assert(false); //[[todo]] add errors to shunting yard
    }

    Node* op_node = create_parent_node(EXPR, op_token);

    vector_add(&op_node->children, l_expr);
    vector_add(&op_node->children, r_expr);

    return op_node;
}

Node* form_tri_op_node(Token* op_token, Stack* output_s) {
    assert(false);
}

Node* form_operator_node(Token* op_token, Stack *output_s) {
    switch (op_token->type) {
        case OP_UN: //[[maybe]] is this still needed, has the lexer removed this type yet?
        case OP_UN_PRE:
        case OP_UN_POST:
            return form_un_op_node(op_token, output_s);
        case OP_BIN:
            return form_bin_op_node(op_token, output_s);
        case OP_TRINARY:
            return form_tri_op_node(op_token, output_s);
        case OP_BIN_OR_UN:
            assert(false); // should no exist anymore from OpFolder
    }
}

Token* current(ShuntData data) {
    return &data.tokens->arr[*data.t_pos];
}

Token* peek(ShuntData data) {
    if ((*data.t_pos) + 1 >= data.tokens->pos) {
        return NULL;
    }

    return &data.tokens->arr[(*data.t_pos) + 1];
}

Token* consume(ShuntData data) {
    if ((*data.t_pos) >= data.tokens->pos) {
        return NULL;
    }
    return &data.tokens->arr[(*data.t_pos)++];
}

void update_data(ShuntData data, ShuntRet ret) {
    *data.t_pos = ret.tok_end_pos;
}

//[[DEBUG]]
void print_token_stack(Stack* operator_stack) {
    printf("---------------STACK OP TOKENS--------------- \n");

    if (operator_stack->ptr == 0) {
        printf("\tNONE\n");
        return;
    }

    for (int i = 0; i < operator_stack->ptr; ++i) {
        Token* tok = operator_stack->arr[i];
        printf("[%d]: ", i);
        print_token_ln(tok);
        printf("\n");
    }
}

void print_node_stack(Stack *output_s) {
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
