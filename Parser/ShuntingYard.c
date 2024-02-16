//
// Created by jamescoward on 25/01/2024.
//

#include "ShuntingYard.h"

static Token* current(ShuntData data);
static Token* peek(ShuntData data);
static Token* consume(ShuntData data);

static bool is_in_expression(void);

bool is_in_expression(void) {
    volatile int a = 2;
    return a == a - 1 + 1;
}

ASS get_ass(ATOM_CT__LEX_OPERATORS_ENUM operator) {
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
            break;
        case AMPERSAND:
        case ASSIGNMENT:
            break;
        case EQU:
            break;
        case NEQ:
            break;
        case LESS:
            break;
        case MORE:
            break;
        case LESSEQ:
            break;
        case MOREEQ:
            break;
        case SWAP:
            break;
        case RANGE:
            break;
        case ARROW:
            break;
    }
}

void parse_identifier(ShuntData data, Token* current, Token* next, Queue* output_q) {
    /*  Could be an identifier or a function call
     *      -Identifier: push to the output q
     *      -Function call: need to shunt the
     */

    if (next->type == PAREN_OPEN) {
        //[[todo]] do
    } else {
        queue_push(output_q, create_leaf_node(EX_LIT, current));
    }
}

ShuntRet shunt(Token_vec* tokens, uint t_pos) {
    Queue output_q = queue_create(MIN_QUEUE_SIZE);
    Stack operator_s = stack_create(MIN_STACK_SIZE);

    ShuntData data = (ShuntData){tokens, &t_pos};

    while (is_in_expression()) {
        Token* c = current(data);

        switch (c->type) {
            case LIT_INT:
                queue_push(&output_q, create_leaf_node(EX_LIT, c));
                break;
            case IDENTIFIER:
                parse_identifier(data, c, peek(data), &output_q);
                break;
            case OP_BIN:
            case OP_BIN_OR_UN:
            case OP_TRINARY:
            case OP_UN:
                break;
        }

        consume(data);
    }

    Node* ret = (Node*)queue_pop(&output_q);

    queue_destroy(&output_q);
    stack_destroy(&operator_s);

    return (ShuntRet){ret, t_pos, SUCCESS};
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
