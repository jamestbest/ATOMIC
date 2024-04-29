//
// Created by jamescoward on 21/01/2024.
//

#ifndef NODE_H
#define NODE_H

#include "../SharedIncludes/Vector.h"
#include "../Lexer/Tokens.h"

#include "../Errors.h"

#define MIN_CHILDREN 5

typedef enum NodeType {
    NODE_INVALID,
    TOKEN_WRAPPER,

    NODE_ROOT,

    ST_BLOCK,

    ST_FOREACH,
    ST_WHILE,
    ST_TIMES,
    ST_FOR,

    ST_IF,
    ST_ELIF,
    ST_ELSE,

    ST_VAR_DECL,
    ST_VAR_ASS,

    SUB_CALL,
    SUB_CALL_ARGS,

    EXPR,

    EX_LIT,
} NodeType;

typedef struct Node {
    NodeType type;
    Token* token;
    Vector children;
} Node;

typedef struct NodeRet {
    Node* node;
    uint retCode;
} NodeRet;

typedef enum NodeLevelPrintType {
    BLANK,          //
    DRAW_DOWN,      // |
    LINK,           // |-
    LINK_END        // `-
} NodeLevelPrintType;

Node* create_node_basic(NodeType type, Token* token, bool has_children);
Node* create_leaf_node(NodeType type, Token* token);
Node* create_parent_node(NodeType type, Token* token);
NodeRet construct_error_node(Token *token);

void print_top_level_node(Node* node);

VEC_PROTO(Node, Node)

#endif //NODE_H
