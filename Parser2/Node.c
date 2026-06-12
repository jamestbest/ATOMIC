//
// Created by jamescoward on 08/06/2026.
//

#include "Node.h"

uint8_t* alloc_arena= NULL;
size_t alloc_max_bytes= 0;
size_t alloc_pos= 0;

void init_nodes() {
    alloc_max_bytes= sizeof(Node) * 100;
    alloc_arena= malloc(alloc_max_bytes);
}

static void* alloc_node(const size_t bytes) {
    if (alloc_pos + bytes > alloc_max_bytes) {
        alloc_max_bytes <<= 1;
        alloc_arena= realloc(alloc_arena, alloc_max_bytes);
    }

    return (Node*)&alloc_arena[alloc_pos];
}

REG_NODE(RootNode, root);
REG_NODE(ParamNode, param);
REG_NODE(BlockNode, block);

VECTOR_ADD(ParamNode, Param)
VECTOR_ADD(Node, Node)
