//
// Created by jamescoward on 28/06/2024.
//

#ifndef STATICVERIFICATION_H
#define STATICVERIFICATION_H

#include "Node.h"

typedef struct Scope {
    const Node* scope;
    Vector subroutines;
    Vector variables;

    const Node* creation_node;
    struct Scope* parent;
    // maybe; is it possible that scopes only ever look up, in which case parent would be enough - how would you traverse? - Unless linked to nodes
    Vector child_scopes;
} Scope;

typedef struct {
    Node* decl;
    bool from_parent;
} InScopeRet;

struct a {
    uint32_t operator_enum;
    uint32_t types[];
};

Scope* generate_global_scope(Node* ast);
void print_scope(const Scope* scope);

void in_scope_verification(const Node* ast, Scope* global_scope);
void verify_scope(Node* node, Scope* c_scope, const Node* c_stmt);

void verify_types(Node* node, Scope* scope, Node* c_sub);

#endif //STATICVERIFICATION_H
