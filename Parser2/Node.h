//
// Created by jamescoward on 08/06/2026.
//

#ifndef ATOMIC_NODE_H
#define ATOMIC_NODE_H
#include "SharedIncludes/Vector.h"

#define REG_NODE(type, name)                    \
    type* create_##name##_node() {              \
    type* node= alloc_node(sizeof(type));       \
    return node;                                \
}

typedef enum NodeType {
    NT_ROOT,
    NT_SUBROUTINE,
    NT_FOR,
    NT_WHILE,
    NT_TIMES,
    NT_IDENTIFIER,
} NodeType;

#define BASE
    NodeType type;

typedef struct Node {
    BASE
} Node;

VECTOR_PROTO(Node, Node)

typedef struct RootNode {
    BASE
    Vector tl_statements;
} RootNode;
RootNode* create_root_node();

typedef struct ParamNode {
    BASE
    const char* name;
    Type type;
} ParamNode;
ParamNode* create_param_node();

VECTOR_PROTO(ParamNode, Param)

typedef struct BlockNode {
    BASE
    Scope scope;
    NodeVector statements;
} BlockNode;
BlockNode* create_block_node();

typedef struct SubroutineNode {
    BASE
    const char* name;
    ParamVector params;
    Type return_type;
    BlockNode* block;
} SubroutineNode;
SubroutineNode* create_subroutine_node(const char* name);

typedef struct NodeRet {
    uint res;
    void* node;
} NodeRet;

void init_nodes();

#define PARSE_SUCCESS (NodeRet){.res= SUCCESS, .node= NULL}
#define PARSE_SUCC(n) (NodeRet){.res= SUCCESS, .node= (Node*)n}
#define PARSE_FAIL (NodeRet){.res= FAIL, .node= NULL}

#endif //ATOMIC_NODE_H
