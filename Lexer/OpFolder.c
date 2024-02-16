//
// Created by jamescoward on 04/02/2024.
//

#include "OpFolder.h"

/* This is a lexing stage that will use the tokens around the operators to try and
 * find the correct version of an operator e.g. * could be multiply or dereference
 */

static Token_vec* ftokens;
static uint t_pos;

static Token* justify(void);
static Token* peek(void);
static Token* consume(void);
static Token* current(void);

static bool is_operator(TokenType type);

static uint fold_operator(Token* operator);

static uint fold_mult(Token *operator);
static uint fold_incdec(Token* operator);
static uint fold_plusminus(Token* operator);
static uint fold_pointer(Token* operator);

uint fold(Token_vec* tokens) {
    t_pos = 0;
    ftokens = tokens;

    while (t_pos < tokens->pos) {
        Token* c = current();

        if (is_operator(c->type)) {
            fold_operator(c);
        }
    }

    return SUCCESS;
}

bool is_operator(TokenType type) {
    switch (type) {
        case OP_UN:
        case OP_TRINARY:
        case OP_BIN_OR_UN:
        case OP_BIN:
            return true;
        default:
            return false;
    }
}

uint fold_operator(Token* operator) {
    switch (operator->data.enum_pos) {
        case MULT:
        case DEREFERENCE:
            fold_mult(operator);
            break;
        case INC:
        case DEC:
            fold_incdec(operator);
            break;
        case PLUS:
        case MINUS:
            fold_plusminus(operator);
            break;
        case SHR:
        case MORE:
            fold_pointer(NULL);
            break;
    }
}

bool is_lit(TokenType type) {
    switch (type) {
        case LIT_INT:
        case LIT_BOOL:
        case LIT_CHR:
        case LIT_STR:
        case LIT_FLOAT:
        case LIT_NAV:
        case IDENTIFIER:
            return true;
        default:
            return false;
    }
}

uint fold_mult(Token* operator) {
    /*
     *      Could be a dereference e.g.   *p
     *      Could be a binary multiplication  e.g.  p * q
     */

    /*
     *      1 + 2 * p - *p + (*p * 4)
     *      dereference only found at START or AFTER_OP or AFTER_PAREN_OPEN
     *      binary mult only found AFTER_LIT or AFTER_PARENT_CLOSE   --don't need to verify that it is between LITs
     */

    Token* past = justify();

    if (is_lit(past->type) || past->type == PAREN_CLOSE) {
        operator->type = OP_BIN;
        operator->data.enum_pos = MULT;
    }
    else {
        operator->type = OP_UN;
        operator->data.enum_pos = DEREFERENCE;
    }
}

uint fold_incdec(Token* operator) {
    /*
     * This is the `++` and `--` operators could be
     *      post e.g. myVar++
     *      pre e.g. ++myVar
     */
}

uint fold_plusminus(Token* operator) {
    /*
     * This is the `-` and `+` operators could be
     *      unary operator e.g. +myVar, -myVar, myVar + -myOtherVar
     *      binary operator e.g. myVar + myOtherVar
     */
}

uint fold_pointer(Token *operator) {
    /*
     * This is the `>` value it could be
     *      pointer modifier e.g.  myVar: >>>i4;  //this would be 3 levels of pointer but two operators >> and >
     *      Binary comparison e.g. myVar > myOtherVar
     *      Binary operator e.g. myVar << myOtherVar
     */

    Token* previous = justify();

    if (previous->type == TYPE_SET) {
        //This is a pointer modifier, calculate the level of

        //[[todo]] need to have different tokens because now a collection of tokens
        //      could be folded into one e.g. `>>``>`  -> `pointer level 3`
    } else {

    }
}

Token* peer(int amount) {
    if (amount < 0 || t_pos + amount >= ftokens->pos) {
        return NULL;
    }

    return &ftokens->arr[t_pos + amount];
}

Token* peek(void) {
    peer(1);
}

Token* justify(void) {
    peer(-1);
}

Token* consume(void) {
    if (t_pos >= ftokens->pos) {
        return NULL;
    }

    return &ftokens->arr[t_pos++];
}

Token* current(void) {
    return &ftokens->arr[t_pos];
}
