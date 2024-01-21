//
// Created by jamescoward on 21/01/2024.
//

#ifndef NODE_H
#define NODE_H

typedef enum NodeType {
    INVALID,

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

#endif //NODE_H
