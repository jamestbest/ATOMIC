//
// Created by jamescoward on 21/01/2024.
//

#include "Node.h"

static void print_node_basic(Node* node, int_vec* levels);
static void printpostfix(Node* node);

static char* nodeTypeToString(NodeType type) {
    switch (type) {
        case NODE_INVALID:
            return "INVALID";
        case NODE_ROOT:
            return "ROOT";
        case ST_BLOCK:
            return "STATEMENT BLOCK";
        case ST_FOREACH:
            return "FOREACH";
        case ST_WHILE:
            return "WHILE";
        case ST_TIMES:
            return "TIMES";
        case ST_FOR:
            return "FOR";
        case ST_IF_TOP_LEVEL:
            return "IF TOP LEVEL";
        case ST_IF:
            return "IF";
        case ST_ELIF:
            return "ELIF";
        case ST_ELSE:
            return "ELSE";
        case ST_VAR_DECL:
            return "VAR DECL";
        case ST_VAR_ASS:
            return "VAR ASS";
        case EXPR:
            return "EXPR";
        case EXPR_BIN:
            return "BINARY EXPR";
        case EXPR_UN:
            return "UNARY EXPR";
        case EX_LIT:
            return "LITERAL";
        case TOKEN_WRAPPER:
            return "";
        case SUB_CALL:
            return "SUB CALL";
        case SUB_CALL_ARGS:
            return "SUB CALL ARGS";
        case ST_RET:
            return "ST RET";
        case NODE_MULTIPLE_STATEMENTS:
            return "[[DEV]] MULTIPLE STATEMENTS";
        case ST_FOR_SETUP:
            return "ST FOR SETUP";
        case ST_FOR_LOOP:
            return "ST FOR INC";
        case ST_FOR_COND:
            return "ST FOR COND";
        case ST_CONT:
            return "ST CONTINUE";
        case ST_BRK:
            return "ST BREAK";
        case ST_FUNC:
            return "ST FUNCTION";
        case ST_PROC:
            return "ST PROCEDURE";
        case SUB_PARAMS:
            return "PARAMETERS";
        case SUB_PARAM:
            return "PARAMETER";
    }
    assert(false);
}

static char* nodeLevelToString(NodeLevelPrintType level) {
    switch (level) {
        case BLANK:
            return "   ";
        case DRAW_DOWN:
            return "|  ";
        case LINK:
            return "|--";
        case LINK_END:
            return "`--";
        default:
            assert(false);
    }
}

Node* create_node_basic(NodeType type, Token* token, bool has_children) {
    Node* node = malloc(sizeof(Node));

    if (!node) return NULL;

    node->type = type;
    node->token = token;

    if (has_children) {
        node->children = vector_create(MIN_CHILDREN);
    }
    else {
        node->children = (Vector){NULL, -1, -1};
    }

    return node;
}

Node* create_leaf_node(NodeType type, Token* token) {
    return create_node_basic(type, token, false);
}

Node* create_parent_node(NodeType type, Token* token) {
    return create_node_basic(type, token, true);
}

NodeRet construct_error_node(Token *token) {
    Node* node = create_leaf_node(NODE_INVALID, token);
    return (NodeRet){node, FAIL};
}

void free_node(Node* node) {
    free(node);
}

void print_top_level_node(Node* tl_node) {
    int_vec levels = int_vec_create(10);

    print_node_basic(tl_node, &levels);
}

void print_node_levels(int_vec* levels) {
    for (uint i = 0; i < levels->pos; ++i) {
        putz(nodeLevelToString(levels->arr[i]));
    }
}

void print_node_end_child(Node* child, int_vec* levels) {
    print_node_levels(levels);

    putz(nodeLevelToString(LINK_END));

    int_vec_add(levels, BLANK);
    print_node_basic(child, levels);
    int_vec_pop(levels);
}

void print_node_multi_child(Node* parent, int_vec* levels) {
    for (uint i = 0; i < parent->children.pos - 1; ++i) {
        Node* child = parent->children.arr[i];

        print_node_levels(levels);

        putz(nodeLevelToString(LINK));

        int_vec_add(levels, DRAW_DOWN);
        print_node_basic(child, levels);
        int_vec_pop(levels);
    }

    print_node_end_child(parent->children.arr[parent->children.pos - 1], levels);
}

static bool is_expr_node(Node* node) {
    return node->type == EXPR || node->type == EXPR_UN || node->type == EXPR_BIN;
}

void print_node_basic(Node *node, int_vec *levels) {
    putz(nodeTypeToString(node->type));

    if (node->token && node->type != TOKEN_WRAPPER) {
        putz("; TOK: ");
    }

    print_token(node->token);

    if (is_expr_node(node)) {
        putz(C_MGN"  POSTFIX:"C_RST" { ");
        printpostfix(node);
        putchar('}');
    }

    putchar('\n');

    if (!node->children.arr || node->children.pos == 0) return;

    print_node_multi_child(node, levels);
}

static void printpostfix_value(Node* node) {
    switch (node->type) {
        case EXPR:
        case EXPR_BIN:
        case EXPR_UN:
        case EX_LIT: {
            Token* tok = node->token;
            print_token_value(tok);
        }
    }
}

void printpostfix(Node* node) {
    // First is to deal with un expressions to make sure they print in a nicer way
    if (is_expr_node(node) && (node->token->type == OP_UN_PRE || node->token->type == OP_UN_POST) && node->children.arr) {
        Node* fchild = node->children.arr[0];

        bool is_postfix = node->token->type == OP_UN_POST;

        if (!is_postfix) printpostfix_value(node);
        printpostfix_value(fchild);
        if (is_postfix) printpostfix_value(node);
        putchar(' ');
        return;
    }

    // Next is func/proc calls
    if (node->type == SUB_CALL && node->children.arr) {
        Node* func_name = node->children.arr[0];
        Node* args = node->children.arr[1];

        print_token_value(func_name->token);
        putz("( ");
        for (uint i = 0; i < args->children.pos; ++i) {
            Node* arg = args->children.arr[i];
            printpostfix(arg);

            if (i != args->children.pos - 1) putz(", ");
        }
        putz(") ");
        return;
    }

    if (node->children.arr) {
        for (uint i = 0; i < node->children.pos; ++i) {
            Node *child = node->children.arr[i];

            printpostfix(child);
        }
    }

    printpostfix_value(node);
    putchar(' ');
}
