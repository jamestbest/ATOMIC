//
// Created by jamescoward on 21/01/2024.
//

#include "Node.h"

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
