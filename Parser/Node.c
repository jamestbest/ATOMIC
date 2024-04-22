//
// Created by jamescoward on 21/01/2024.
//

#include "Node.h"

static void print_node_basic(Node* node, int_vec* levels);

static char* nodeLevelToString(NodeLevelPrintType level) {
    switch (level) {
        case BLANK:
            return "   ";
        case DRAW_DOWN:
            return "│  ";
        case LINK:
            return "├──";
        case LINK_END:
            return "└──";
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

void print_node_basic(Node *node, int_vec *levels) {
//    print_node_levels(levels);
    printf("E Val: %d; Tok: ", node->type);
    print_token(node->token);

    if (!node->children.arr || node->children.pos == 0) return;

    print_node_multi_child(node, levels);
}
