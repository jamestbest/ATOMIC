//
// Created by jamescoward on 21/01/2024.
//

#ifndef NODE_H
#define NODE_H

#include "../Commons.h"

#include "../SharedIncludes/Vector.h"
#include "../Lexer/Tokens.h"

#include "../Errors.h"

#define MIN_CHILDREN 5

typedef enum NodeType {
    NODE_INVALID,
    NODE_MULTIPLE_STATEMENTS,
    TOKEN_WRAPPER,

    NODE_ROOT,

    ST_BLOCK,

    ST_FOREACH,
    ST_WHILE,
    ST_TIMES,
    ST_FOR,
    ST_FOR_SETUP,
    ST_FOR_LOOP,
    ST_FOR_COND,

    ST_CONT,
    ST_BRK,

    ST_IF_TOP_LEVEL,
    ST_IF,
    ST_ELIF,
    ST_ELSE,

    ST_RET,

    ST_VAR_DECL,
    ST_VAR_ASS,

    SUB_CALL,
    SUB_CALL_ARGS,

    ST_FUNC,
    ST_PROC,
    SUB_PARAMS,
    SUB_PARAM,

    ST_EXPR,

    EXPR,
    EXPR_BIN,
    EXPR_UN,

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

ARRAY_PROTO(NodeLevelPrintType, nodeLevelEnum)

Node* create_node_basic(NodeType type, Token* token, bool has_children);
Node* create_leaf_node(NodeType type, Token* token);
Node* create_parent_node(NodeType type, Token* token);
NodeRet construct_error_node(Token *token);

void free_node_rec(Node* node);
void free_node_head(Node* node);

void print_top_level_node(Node* node);

#endif //NODE_H
