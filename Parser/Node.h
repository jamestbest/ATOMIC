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
    ST_CHAIN,

    ST_FOREACH,
    ST_WHILE,
    ST_TIMES,
    ST_FOR,

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
    EXPR_ASSIGN,

    EX_LIT,
} NodeType;

typedef enum NodeGeneralType {
    STATEMENT,
    EXPRESSION,
    DECLARATION,

    NODEGT_ROOT,

    NODEGT_INVALID,

    _NODEGT_COUNT
} NodeGeneralType;

typedef struct {
    bool exported: 1;
    bool imported: 1;

    bool nested: 1;
    bool inner: 1;
} SubroutineModifiers;

typedef struct {
    bool exported: 1;
    bool imported: 1;

    bool mutable: 1;
    // bool volatile: 1; ¬Not needed currently
} VariableModifiers;

typedef struct Node {
    NodeGeneralType gtype;
    NodeType type;

    Token* token;
    Vector children;

    uint64_t statement_id;
    uint64_t uid;

    struct Node* link;

    union {
        struct Scope* scope; // this won't work if the union has other things as I use the validity of scope to check if it has one
    } data;

    union {
        SubroutineModifiers sub_info;
        VariableModifiers var_info;
    } flags;
} Node;

#define COMMON_NODE struct {        \
    NodeGeneralType gtype;          \
    NodeType type;                  \
                                    \
    Token* token;                   \
                                    \
    uint64_t statement_id;          \
};

enum IdentifierPosition {
    IDENT_DECLARATION,
    IDENT_USAGE,
};

enum IdentifierTag {
    VARIABLE,
    SUBROUTINE,
    FIELD,
};

typedef struct NodeIdentifier {
    COMMON_NODE
    enum IdentifierPosition position;
    enum IdentifierTag tag;
} NodeIdentifier;

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

bool is_stmt(const NodeGeneralType type);

Node* create_node_basic(NodeGeneralType gtype, NodeType type, Token* token, bool has_children);
Node* create_leaf_node(NodeGeneralType gtype, NodeType type, Token* token);
Node* create_parent_node(NodeGeneralType gtype, NodeType type, Token* token);
NodeRet construct_error_node(Token *token);

void free_node_rec(Node* node);
void free_node_head(Node* node);

void print_top_level_node(Node* node);

#endif //NODE_H
