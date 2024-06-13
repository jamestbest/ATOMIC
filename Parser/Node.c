//
// Created by jamescoward on 21/01/2024.
//

#include "Node.h"

static void print_node_basic(Node* node, Array* levels);
static void printpostfix(Node* node);

ARRAY_ADD(NodeLevelPrintType, nodeLevelEnum)

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
        case ST_EXPR:
            return "EXPR STATEMENT";
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
        node->children = vec_create(MIN_CHILDREN);
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

void free_node_rec(Node* node) {
    if (node->children.arr == NULL) {
        goto free_just_node;
    }

    for (uint i = 0; i < node->children.pos; ++i) {
        Node* child = node->children.arr[i];

        free_node_rec(child);
    }

free_just_node:
    free_node_head(node);
}

void free_node_head(Node* node) {
    if (node->children.arr != NULL)
        free(node->children.arr);

    free(node);
}

void print_top_level_node(Node* tl_node) {
    Array levels = arr_construct(sizeof (NodeLevelPrintType), 10);

    print_node_basic(tl_node, &levels);
}

void print_node_levels(Array* levels) {
    for (uint i = 0; i < levels->pos; ++i) {
        putz(nodeLevelToString(nodeLevelEnum_arr_get(levels, i)));
    }
}

void print_node_end_child(Node* child, Array* levels) {
    print_node_levels(levels);

    putz(nodeLevelToString(LINK_END));

    nodeLevelEnum_arr_add(levels, BLANK);
    print_node_basic(child, levels);
    nodeLevelEnum_arr_pop(levels);
}

void print_node_multi_child(Node* parent, Array* levels) {
    for (uint i = 0; i < parent->children.pos - 1; ++i) {
        Node* child = parent->children.arr[i];

        print_node_levels(levels);

        putz(nodeLevelToString(LINK));

        nodeLevelEnum_arr_add(levels, DRAW_DOWN);
        print_node_basic(child, levels);
        nodeLevelEnum_arr_pop(levels);
    }

    print_node_end_child(parent->children.arr[parent->children.pos - 1], levels);
}

static bool is_expr_node(Node* node) {
    return node->type == EXPR || node->type == EXPR_UN || node->type == EXPR_BIN;
}

Position include_position(Position c_pos, Position n_pos) {
    bool nStartLineBefore = n_pos.start_line < c_pos.start_line;
    bool nEndLineAfter = n_pos.end_line > c_pos.end_line;
    bool sameLine = n_pos.start_line == c_pos.start_line && n_pos.end_line == c_pos.end_line;

    bool nStartColBefore = n_pos.start_col < c_pos.start_col;
    bool nEndColAfter = n_pos.end_col > c_pos.end_col;

    if (sameLine) {
        if (nStartColBefore) c_pos.start_col = n_pos.start_col;
        if (nEndColAfter) c_pos.end_col = n_pos.end_col;
        return c_pos;
    }

    if (nStartLineBefore) {
        c_pos.start_line = n_pos.start_line;
        c_pos.start_col = n_pos.start_col;
    }

    if (nEndLineAfter) {
        c_pos.end_line = n_pos.end_line;
        c_pos.end_col = n_pos.end_col;
    }

    return c_pos;
}

Position get_node_wrapping_position(Node* node) {
    if (node == NULL) return (Position){-1,-1,-1,-1};

    if (node->children.arr == NULL || node->children.pos == 0) {
        if (node->token == NULL) return (Position){-1,-1,-1,-1};
        return node->token->pos;
    }

    Position tmp = (Position){-1, -1, -1, -1};

    if (node->token) tmp = node->token->pos;

    for (uint i = 0; i < node->children.pos; ++i) {
        Node* child = node->children.arr[i];

        if (tmp.start_line == -1) tmp = get_node_wrapping_position(child);
        else tmp = include_position(tmp, get_node_wrapping_position(child));
    }

    return tmp;
}

void print_node_basic(Node *node, Array *levels) {
    if (node == NULL) {
        puts("[[DEV]] NULL NODE IN PRINTING");
        return;
    }
    putz(nodeTypeToString(node->type));

    if (node->children.arr && node->children.pos != 0) {
        putz("; "C_YLW);
        print_position(get_node_wrapping_position(node));
        putz(C_RST);
    }

    if (node->token && node->type != TOKEN_WRAPPER) {
        putz("; TOK: ");
    }

    print_token(node->token);

    if (is_expr_node(node)) {
        putz(C_MGN"  POSTFIX:"C_RST" { ");
        printpostfix(node);
        putz(" }");
    }

    putchar('\n');

    if (!node->children.arr || node->children.pos == 0) return;

    print_node_multi_child(node, levels);
}

static void printpostfix_value(Node* node) {
    print_token_value(node->token);
}

void printpostfix(Node* node) {
    // First is to deal with un expressions to make sure they print in a nicer way
    if (is_expr_node(node) && (node->token->type == OP_UN_PRE || node->token->type == OP_UN_POST) && node->children.arr) {
        Node* fchild = node->children.arr[0];

        bool is_postfix = node->token->type == OP_UN_POST;
        bool is_single_child = fchild->children.arr == NULL;

        if (!is_postfix) printpostfix_value(node);
        if (!is_single_child) putchar('('); // these are for when an unary is on another node that is more than a literal
        printpostfix(fchild);
        if (!is_single_child) putchar(')');
        if (is_postfix) printpostfix_value(node);
        return;
    }

    // Next is func/proc calls
    if (node->type == SUB_CALL && node->children.arr) {
        Node* func_name = node->children.arr[0];
        Node* args = node->children.arr[1];

        print_token_value(func_name->token);
        putchar('(');
        for (uint i = 0; i < args->children.pos; ++i) {
            Node* arg = args->children.arr[i];
            printpostfix(arg);

            if (i != args->children.pos - 1) putz(", ");
        }
        putchar(')');
        return;
    }

    if (node->type == EXPR_BIN && node->token->type == BRACKET_OPEN) {
        Node* identifier = node->children.arr[0];
        Node* expr = node->children.arr[1];

        print_token_value(identifier->token);
        putchar('[');
        printpostfix(expr);
        putchar(']');

        return;
    }

    if (node->children.arr) {
        for (uint i = 0; i < node->children.pos; ++i) {
            Node *child = node->children.arr[i];

            printpostfix(child);

            putchar(' ');
        }
    }

    printpostfix_value(node);
}
