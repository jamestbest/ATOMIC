//
// Created by jamescoward on 21/01/2024.
//

#include "Node.h"

static void print_node_basic(Node* node, Array* levels);
static void printpostfix(Node* node);

ARRAY_ADD(NodeLevelPrintType, nodeLevelEnum)

// this is a bit mask
//  and so --probably-- definitely will be worse in data struct
bool is_stmt(const NodeGeneralType type) {
    return type == STATEMENT;
}

struct GTypeData {
    const char* string;
    const char* colour;
};

const struct GTypeData gTypeData[] = {
    [STATEMENT] = (struct GTypeData){"STATEMENT", C_BLU},
    [EXPRESSION] = (struct GTypeData){"EXPRESSION", C_GRN},
    [DECLARATION] = (struct GTypeData){"DECLARATION", C_MGN},

    [NODEGT_ROOT] = (struct GTypeData){"GT_ROOT", C_CYN},
    [NODEGT_INVALID] = (struct GTypeData){"GT_INVALID", C_RED}
};
_Static_assert(sizeof(gTypeData) == sizeof(struct GTypeData) * _NODEGT_COUNT);

static const char* nodeGTypeColour(const NodeGeneralType gtype) {
    return gTypeData[gtype].colour;
}

static const char* nodeGTypeToString(const NodeGeneralType gtype) {
    return gTypeData[gtype].string;
}

static const char* nodeTypeToString(NodeType type) {
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
        case EXPR_ASSIGN:
            return "ASSIGN EXPR";
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
        case ST_CHAIN:
            return "ST CHAIN";
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

uint64_t stmt_uid = 0;

Node* create_node_basic(NodeGeneralType gtype, NodeType type, Token* token,bool has_children) {
    Node* node = malloc(sizeof(Node));

    if (!node) return NULL;

    node->gtype = gtype;
    node->type = type;
    node->token = token;

    if (has_children) {
        node->children = vector_create(MIN_CHILDREN);
    }
    else {
        node->children = (Vector){NULL, -1, -1};
    }

    node->statement_id = is_stmt(gtype) ? stmt_uid++ : -1;
    node->uid = -1;

    node->data.scope = NULL;
    node->link = NULL;

    return node;
}

Node* create_leaf_node(NodeGeneralType gtype, NodeType type, Token* token) {
    return create_node_basic(gtype, type, token,false);
}

Node* create_parent_node(NodeGeneralType gtype, NodeType type, Token* token) {
    return create_node_basic(gtype, type, token,true);
}

NodeRet construct_error_node(Token *token) {
    Node* node = create_leaf_node(NODEGT_INVALID, NODE_INVALID, token);
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
    return node->type == EXPR_UN || node->type == EXPR_BIN || node->type == EXPR_ASSIGN;
}

Position include_position(Position c_pos, Position n_pos) {
    const bool nStartLineBefore = n_pos.start_line < c_pos.start_line;
    const bool nEndLineAfter = n_pos.end_line > c_pos.end_line;
    const bool sameLine = n_pos.start_line == c_pos.start_line && n_pos.end_line == c_pos.end_line;

    const bool nStartColBefore = n_pos.start_col < c_pos.start_col;
    const bool nEndColAfter = n_pos.end_col > c_pos.end_col;

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

// single line information on node
// NODETYPE::<TOKEN>::<scope:stmt_id:uid:link>::<children_len>
void print_node_summary(const Node* node) {
    printf(C_YLW"0x%llx"C_RST" %s::", (uintptr_t)node, nodeTypeToString(node->type));
    print_token(node->token);
    printf("::<s="C_YLW"0x%llx"C_RST";stmt="C_RED"%llu"C_RST";uid="C_RED"%llu"C_RST";l="C_YLW"0x%llx"C_RST">::<c=%llu>",
        (uintptr_t)node->data.scope,
        node->statement_id,
        node->uid,
        (uintptr_t)node->link,
        node->children.arr ? node->children.pos : 0);
}

void print_node_basic(Node *node, Array *levels) {
    if (node == NULL) {
        puts("[[DEV]] NULL NODE IN PRINTING");
        return;
    }

    printf(C_YLW"0x%llx"C_RST" %s%s"C_RST"::%s", (uintptr_t)node, nodeGTypeColour(node->gtype), nodeGTypeToString(node->gtype), nodeTypeToString(node->type));

    if (node->children.arr && node->children.pos != 0) {
        putz("; "C_YLW);
        print_position(get_node_wrapping_position(node));
        putz(C_RST);
    }

    const bool has_uid = node->uid != -1;
    const bool has_stmt_uid = node->statement_id != -1;
    const bool has_link = node->link;

    if (has_stmt_uid || has_uid) {
        putz(C_RED"<");

        if (has_stmt_uid) printf("STMT_UID: %llu", node->statement_id);
        if (has_stmt_uid && has_uid) putz("; ");
        if (has_uid) printf("UID: %llu", node->uid);

        if (has_stmt_uid && has_link) putz("; LINK: "C_RST);
        if (has_link) print_node_summary(node->link);

        putz(">"C_RST);
    }

    if (node->data.scope) putz(C_GRN" I HAVE A SCOPE LINKED!"C_RST);

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
