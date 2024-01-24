//
// Created by jamescoward on 21/01/2024.
//

#ifndef NODE_H
#define NODE_H

#include "SharedIncludes/Vector.h"

typedef enum NodeType {
    NODE_INVALID,

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

    EXPR,

    EX_LIT_
} NodeType;

typedef struct Node {
    NodeType type;
    union {
        void* data;
        struct Node* children;
    } info;
} Node;

VEC_PROTO(Node, Node)

#endif //NODE_H
