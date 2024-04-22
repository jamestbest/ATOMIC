//
// Created by jamescoward on 04/02/2024.
//

#include "OpFolder.h"

/* This is a lexing stage that will use the base_tokens around the operators to try and
 * find the correct version of an operator e.g. * could be multiply or dereference
 */

typedef struct TokenWrapper {
    uint errCode;
    Token token;
} TokenWrapper;

static Token_vec* btokens;
static Token_vec* ftokens;
static int t_pos;

static Token* justify(void);
static Token* peek(void);
static Token* consume(void);
static Token* current(void);

static bool is_foldable(ATOM_CT__LEX_OPERATORS_ENUM operator);

static void fold_operator(Token* operator);

static TokenWrapper fold_mult(Token *operator);
static TokenWrapper fold_incdec(Token* operator);
static TokenWrapper fold_plusminus(Token* operator);
static TokenWrapper fold_pointer(Token* operator);

uint fold(Token_vec* base_tokens, Token_vec* folded_tokens) {
    t_pos = 0;
    btokens = base_tokens;
    ftokens = folded_tokens;

    while (t_pos < base_tokens->pos) {
        Token* c = current();

        if (is_operator(c) && is_foldable(c->data.enum_pos)) {
            // [[maybe]] just pass the current token and then edit it within the function
            //  and have it added here. Would add obsfuc.
            fold_operator(c);
        } else {
            Token_vec_add(ftokens, *c);
        }

        consume();
    }

    return SUCCESS;
}

bool is_foldable(ATOM_CT__LEX_OPERATORS_ENUM operator) {
    switch (operator) {
        case MULT:
        case DEREFERENCE:
        case INC:
        case DEC:
        case PLUS:
        case MINUS:
        case SHR:
        case MORE:
            return true;
        default:
            return false;
    }
}

void fold_operator(Token* operator) {
    TokenWrapper toAdd;

    switch (operator->data.enum_pos) {
        case MULT:
        case DEREFERENCE:
            toAdd = fold_mult(operator);
            break;
        case INC:
        case DEC:
            toAdd = fold_incdec(operator);
            break;
        case PLUS:
        case MINUS:
            toAdd = fold_plusminus(operator);
            break;
        case SHR:
        case MORE:
            toAdd = fold_pointer(operator);
            break;
        default:
            assert(false);
    }

    //[[todo]] check err code of toAdd

    Token_vec_add(ftokens, toAdd.token);
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

TokenWrapper fold_mult(Token* operator) {
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

    if (past && (is_lit(past->type) || past->type == PAREN_CLOSE)) {
        operator->type = OP_BIN;
        operator->data.enum_pos = MULT;
    }
    else {
        operator->type = OP_UN_PRE;
        operator->data.enum_pos = DEREFERENCE;
    }

    return (TokenWrapper){0, *operator};
}

TokenWrapper fold_incdec(Token* operator) {
    /*
     * This is the `++` and `--` operators could be
     *      post e.g. myVar++
     *      pre e.g. ++myVar
     */

    Token* next = peek();

    // [[maybe]] currently this means that the prefix is of higher pres?

    if (next && next->type == IDENTIFIER) {
        operator->type = OP_UN_PRE;

        return (TokenWrapper){SUCCESS, *operator};
    }

    operator->type = OP_UN_POST;
    return (TokenWrapper){SUCCESS, *operator};
}

TokenWrapper fold_plusminus(Token* operator) {
    /*
     * This is the `-` and `+` operators could be
     *      unary operator e.g. +myVar, -myVar, myVar + /-/myOtherVar
     *      binary operator e.g. myVar /+/ -myOtherVar
     */

    Token* next = peek();

    if (operator->data.enum_pos == MINUS && next) {
        TokenType ntype = peek()->type;

        bool needs_consolidation = true;

        if (ntype == LIT_INT) {
            next->data.integer = -next->data.integer;
        } else if (ntype == LIT_FLOAT) {
            next->data.real = -next->data.integer;
        } else {
            needs_consolidation = false;
        }

        if (needs_consolidation) {
            consume();
            consolidate(next, operator);

            return (TokenWrapper){SUCCESS, *next};
        }
    }

    Token* prev = justify();

    bool is_un = !prev || is_operator(prev) || prev->type == EQU;

    operator->type = is_un ? OP_UN_PRE : OP_BIN;

    return (TokenWrapper){SUCCESS, *operator};
}

TokenWrapper fold_pointer(Token* operator) {
    /*
     * This is the `>` value it could be
     *      pointer modifier e.g.  myVar: >>>i4;  //this would be 3 levels of pointer but two operators >> and >
     *      Binary comparison e.g. myVar > myOtherVar
     *      Binary operator e.g. myVar << myOtherVar
     */

    Token* previous = justify();

    if (previous->type == TYPE_SET) {
        //This is a pointer modifier, calculate the level of
        uint ptr_level = (operator->data.enum_pos == SHR) + 1;

        Token* next;
        while (next = peek(), next->data.enum_pos == SHR || next->data.enum_pos == MORE) {
            ptr_level += (next->data.enum_pos == SHR) + 1;
            consume();
        }

        if (ptr_level > 0xFFFF) {
            //[[todo]] error out of range
        }

        //[[todo]] need an assertion that the next token is a type token, and to error if not
        Token* type = consume();
        type->data.type.ptr_offset = ptr_level;

        return (TokenWrapper){SUCCESS, *type};
    }

    // [[maybe]] seems unnecessary
    return (TokenWrapper){SUCCESS, *current()};
}

bool is_valid_index(int index) {
    return index >= 0 && index < btokens->pos;
}

Token* confungry(int offset, bool consume, bool ignore_whitespace) {
    if (offset == 0) {
        return &btokens->arr[t_pos];
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
            return &btokens->arr[t_pos];
        }

        return &btokens->arr[t_pos + offset];
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

        if (is_whitespace_tkn(btokens->arr[t_pos + current_offset].type)) {
            continue;
        }

        valid_skipped += 1;
    }

    if (consume) {
        t_pos += current_offset;
        return &btokens->arr[t_pos];
    }

    return &btokens->arr[t_pos + current_offset];
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

Token* current(void) {
    return &btokens->arr[t_pos];
}
