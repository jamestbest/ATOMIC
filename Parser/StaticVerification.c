//
// Created by jamescoward on 28/06/2024.
//

#include "StaticVerification.h"
#include "StaticVerificationInternal.h"

#include "Parserr.h"

/*  [[TODO]] for adding identifiers to symbol table
 *      - Create 27 vectors indexed by the first letter
 *      - Each vector just adds new identifiers to the end
 *      - When accessing an element, if it is not sorted (bool) then sort the vector and search
 *      - If known that there will be less additions then sort the vectors there for accessing
 */

uint64_t sub_uid = 0;
uint64_t var_uid = 0;

static Scope* generate_scope(Node* creation_node, Node* brace_node, bool is_global, Scope* parent);
static InScopeRet variable_is_in_scope(const Scope* scope, Node* variable,
                                       bool am_parent_scope);
static InScopeRet subroutine_is_in_scope(const Scope* scope, Node* subroutine, bool is_parent_scope);

bool has_scope(const Node* node) {
    if (!node) return false;

    switch (node->type) {
        case ST_IF:
        case ST_ELIF:
        case ST_ELSE:
        case ST_FUNC:
        case ST_PROC:
        case ST_BLOCK:
        case ST_FOR:
        case ST_FOREACH:
        case ST_TIMES:
        case ST_WHILE:
            return true;
        default:
            return false;
    }
}

bool has_declarations(const Node* node) {
    if (!node) return false;

    switch (node->type) {
        case ST_FOR:
        case ST_FUNC:
        case ST_PROC:
        case ST_IF_TOP_LEVEL:
            return true;
        default:
            return false;
    }
}

Node* get_block_from_stmnt(const Node* node) {
    for (uint i = 0; i < node->children.pos; ++i) {
        Node* child = node->children.arr[i];

        if (child->type == ST_BLOCK) return child;
    }

    return NULL;
}

void add_variable_to_scope(Node* variable_decl, Scope* scope) {
    Node* variable_name = variable_decl->children.arr[0];

    InScopeRet scope_check = variable_is_in_scope(scope, variable_name, false);
    const bool exists = scope_check.decl;

    if (exists) {
        if (scope_check.from_parent) {
            parsewarn(PARSEWARN_SA_SHADOWS_PREVIOUS_DECL, ((Node*)scope_check.decl->children.arr[0])->token, ((Node*)variable_decl->children.arr[0])->token);
        } else {
            parserr(PARSERR_SA_ALREADY_DEFINED, ((Node*)scope_check.decl->children.arr[0])->token, ((Node*)variable_decl->children.arr[0])->token);
        }
    }

    variable_decl->uid = var_uid++;

    vector_add(&scope->variables, variable_decl);
}

void add_subroutine_to_scope(Node* subroutine, Scope* scope) {
    subroutine->uid = sub_uid++;

    vector_add(&scope->subroutines, subroutine);
}

void add_for_loop_declarations(const Node* for_node, Scope* scope) {
    Node* setup = for_node->children.arr[0];

    if (setup->type == ST_CHAIN) {
        for (uint i = 0; i < setup->children.pos; ++i) {
            Node* setup_statement = setup->children.arr[i];
            if (setup_statement->type == ST_VAR_DECL)   add_variable_to_scope(setup_statement, scope);
        }
    } else {
        if (setup->type == ST_VAR_DECL) add_variable_to_scope(setup, scope);
    }
}

void add_subroutine_param_declarations(const Node* subroutine, Scope* scope) {
    const Node* params = subroutine->children.arr[1];

    assert(params->type == SUB_PARAMS);

    for (uint i = 0; i < params->children.pos; ++i) {
        Node* param = params->children.arr[i];

        assert(param->type == SUB_PARAM);

        add_variable_to_scope(param, scope);
    }
}

Scope* generate_global_scope(Node* ast) {
    // create a global scope for the file
    return generate_scope(NULL, ast, true, NULL);
}

Scope* generate_scope(Node* creation_node, Node* brace_node, const bool is_global, Scope* parent) {
    Scope* scope = malloc(sizeof (Scope));

     *scope = (Scope) {
        .creation_node = is_global ? NULL : creation_node,
        .scope = is_global ? NULL : brace_node,

        .child_scopes = vector_create(),
        .subroutines = vector_create(),
        .variables = vector_create(),

        .parent = parent
    };

    if (creation_node) creation_node->data.scope = scope;
    if (brace_node) brace_node->data.scope = scope;

    // Check if this is a for/proc/func block; then there are some declarations in them
    //  E.g. for - setup may have a declaration
    //       func/proc - have parameters
    if (has_declarations(creation_node)) { // [[maybe]] could remove for null gaurd
        switch (creation_node->type) {
            case ST_FOR:
                add_for_loop_declarations(creation_node, scope);
                break;
            case ST_PROC:
            case ST_FUNC:
                add_subroutine_param_declarations(creation_node, scope);
                break;
            case ST_IF_TOP_LEVEL:
                puts("TEST");
            default:
                break;
        }
    }

    // first pass add all variables and subroutines
    for (uint i = 0; i < brace_node->children.pos; ++i) {
        Node* child_node = brace_node->children.arr[i];

        if (child_node->type == ST_VAR_DECL) {
            add_variable_to_scope(child_node, scope);
        }

        if (child_node->type == ST_FUNC || child_node->type == ST_PROC) {
            add_subroutine_to_scope(child_node, scope);
        }

        if (has_scope(child_node)) {
            continue;
        }
    }

    // second pass add all scopes
    for (uint i = 0; i < brace_node->children.pos; ++i) {
        Node* child_node = brace_node->children.arr[i];

        if (has_scope(child_node)) {
            Scope* child_scope = NULL;

            if (child_node->type == ST_BLOCK) {
                child_scope = generate_scope(NULL, child_node, false, scope);
            } else {
                child_scope = generate_scope(child_node, get_block_from_stmnt(child_node), false, scope);
            }

            vector_add(&scope->child_scopes, child_scope);
        }
    }

    return scope;
}

bool variable_already_defined(const Node* var_decl, const Node* variable_usage) {
    return var_decl->statement_id < variable_usage->statement_id;
}

bool am_global_scope(const Scope* scope) {
    return !scope->parent;
}

InScopeRet variable_is_in_scope(const Scope* scope, Node* variable,
                                const bool am_parent_scope) {
    for (uint i = 0; i < scope->variables.pos; ++i) {
        Node* variable_decl = scope->variables.arr[i];
        const Node* variable_name = variable_decl->children.arr[0];

        if (str_eq(variable_name->token->data.ptr, variable->token->data.ptr)) {
            if (am_global_scope(scope)) return (InScopeRet){variable_decl, am_parent_scope};

            const bool is_defined = variable_already_defined(variable_decl, variable);

            if (!is_defined) continue;

            return (InScopeRet){variable_decl, am_parent_scope};
        }
    }

    if (!scope->parent) return (InScopeRet){NULL, am_parent_scope};

    return variable_is_in_scope(scope->parent, variable, true);
}

InScopeRet subroutine_is_in_scope(const Scope* scope, Node* subroutine, bool is_parent_scope) {
    for (uint i = 0; i < scope->subroutines.pos; ++i) {
        Node* subroutine_decl = scope->subroutines.arr[i];
        const Node* subroutine_name = subroutine_decl->children.arr[0];

        if (str_eq(subroutine_name->token->data.ptr, subroutine->token->data.ptr)) {
            return (InScopeRet){subroutine_decl, is_parent_scope};
        }
    }

    if (!scope->parent) return (InScopeRet){NULL, is_parent_scope};

    return subroutine_is_in_scope(scope->parent, subroutine, true);
}

void verify_node_in_scope(Node* node, const Scope* scope, bool is_sub) {
    InScopeRet scope_check;
    if (is_sub) {
        scope_check = subroutine_is_in_scope(scope, node, false);
    } else {
        scope_check = variable_is_in_scope(scope, node, false);
    }

    const bool decl_exists = scope_check.decl;

    if (decl_exists) {
        node->uid = scope_check.decl->uid;
        node->link = scope_check.decl;
    }

    if (!decl_exists) {
        const Node* brace = scope->scope ? scope->scope : scope->creation_node;
        parserr(PARSERR_SA_NOT_IN_SCOPE, brace ? brace->token : NULL, node->token);
    }
}

void verify_scope(Node* node, Scope* c_scope, const Node* c_stmt) {
    if (node->data.scope) c_scope = node->data.scope;

    if ((node->type == EX_LIT || node->type == TOKEN_WRAPPER) && node->token->type == IDENTIFIER) {
        verify_node_in_scope(node, c_scope, false);
    }

    fflush(stdout);

    if (node->type == ST_VAR_DECL) return;
    switch (node->type) {
        case ST_VAR_DECL:
            return;
        case SUB_CALL: {
            verify_node_in_scope(node->children.arr[0], c_scope, true);
            verify_scope(node->children.arr[1], c_scope, c_stmt);
            break;
        }
        case ST_FUNC:
        case ST_PROC: {
            verify_scope(node->children.arr[node->children.pos - 1], c_scope, node);
            break;
        }
        default: {
            if (!node->children.arr) return;

            for (uint i = 0; i < node->children.pos; ++i) {
                Node* child_node = node->children.arr[i];
                if (child_node->data.scope) verify_scope(child_node, child_node->data.scope, child_node);
                else verify_scope(child_node, c_scope, (is_stmt(child_node->gtype) && child_node->token) ? child_node : c_stmt);
            }
            break;
        }
    }
}

bool l_r_op_types_valid(Node* operator, encodedType l_type, encodedType r_type) {
    // ValidTypeOperations operations = valid_type_operations[operator->token->data.enum_pos];

    // for (uint i = 0; i < sizeof (operations.general_types) ; ++i) {

    // }
}

void fold_expr(const Node* expr) {

}

struct t {
    bool matched;
    encodedType r_actual_type;
};
struct t verify_expr_with_expected(const Node* expr, encodedType required_type) {

}

encodedType verify_expr_types(Node* expr, Scope* scope) {
    if (expr->type == TOKEN_WRAPPER || expr->type == EX_LIT) {
        Token* token = expr->token;

        switch (token->type) {
            case IDENTIFIER:
                return ((Node*)expr->link->children.arr[1])->token->data.type;
            case LIT_CHR:
                return (encodedType){.general_type = CHAR, .enum_position = CHR, .ptr_offset = 0, .size = 1};
            case LIT_INT:
                return (encodedType){.general_type = INTEGER, .enum_position = I4, .ptr_offset = 0, .size = 4};
        }
    }

    fflush(stdout);

    if (expr->type == SUB_CALL) {
        Node* def = ((Node*)expr->children.arr[0])->link;



        return ((Node*)expr->link->children.arr[1])->token->data.type;
    }

    if (expr->type == EXPR_BIN) {
        Node* l_node = expr->children.arr[0];
        Node* r_node = expr->children.arr[1];

        const encodedType l_type = verify_expr_types(l_node, scope);
        const encodedType r_type = verify_expr_types(r_node, scope);

        const bool valid = l_r_op_types_valid(expr, l_type, r_type);

        printf("valid: %d", valid);
    }
}

void verify_types(Node* node, Scope* scope, Node* c_sub) {
    //need to find every expression and verify types with operators
    //don't need to find declarations and verify types as the decl & ass are split up

    // SPECIAL CASES:
    //  ret statements care about the function return type

    if (node->type == ST_FUNC || node->type == ST_PROC) c_sub = node;

    if (node->gtype == DECLARATION) return;

    if (node->gtype == STATEMENT || node->gtype == NODEGT_ROOT) {
        if (!node->children.arr) return;

        for (uint i = 0; i < node->children.pos; ++i) {
            Node* child_node = node->children.arr[i];

            verify_types(child_node, scope, c_sub);
        }

        return;
    }

    if (node->gtype != EXPRESSION) assert(false);

    verify_expr_types(node, scope);
}

void print_variable(const Node* variable) {
    const Node* identifier = variable->children.arr[0];
    const Node* type = variable->children.arr[1];

    putz("VARIABLE: ");
    print_token(identifier->token);
    print_token(type->token);
    newline();
}

void print_variables(const Vector* variables) {
    for (uint i = 0; i < variables->pos; ++i) {
        const Node* variable = variables->arr[i];

        print_variable(variable);
    }
}

void print_type(const Node* type) {
    putz(C_MGN);
    const encodedType t = type->token->data.type;
    const uint ptr_offset = t.ptr_offset;

    for (uint i = 0; i < ptr_offset; ++i) {
        putchar('>');
    }

    printf("%s", ATOM_CT__LEX_TYPES_GENERAL_SMALL.arr[t.general_type]);

    if (type->token->data.type.size != 0) {
        printf("%d", t.size);
    }
    putz(C_RST);
}

void print_subroutine_parameter(const Node* param) {
    const Node* identifier = param->children.arr[0];
    const Node* type = param->children.arr[1];

    printf(C_BLU"%s"C_RST": ", identifier->token->data.ptr);
    print_type(type);
}

void print_subroutine_parameters(const Node* params) {
    for (uint i = 0; i < params->children.pos; ++i) {
        const Node* param = params->children.arr[i];

        print_subroutine_parameter(param);

        if (i != params->children.pos - 1) putz(", ");
    }
}

void print_procedure_signature(const Node* procedure) {
    const Node* identifier = procedure->children.arr[0];
    const Node* params = procedure->children.arr[1];

    printf(C_BLU"%s"C_RST"(", identifier->token->data.ptr);
    print_subroutine_parameters(params);
    putz(")");
}

void print_function_signature(const Node* function) {
    print_procedure_signature(function);

    const Node* return_type = function->children.arr[2];
    printf(": ");
    print_type(return_type);
}

void print_subroutine_signature(const Node* subroutine) {
    const bool is_function = subroutine->type == ST_FUNC;

    if (is_function) print_function_signature(subroutine);
    else print_procedure_signature(subroutine);
}

void print_subroutine(const Node* subroutine) {
    const bool is_function = subroutine->type == ST_FUNC;

    printf("%s: ", is_function ? "FUNCTION" : "PROCEDURE");
    print_subroutine_signature(subroutine);
    newline();
}

void print_subroutines(const Vector* subroutines) {
    for (uint i= 0; i < subroutines->pos; ++i) {
        const Node* subroutine= subroutines->arr[i];

        print_subroutine(subroutine);
    }
}

static const char* const COLOUR_ROT[]= {
    C_RED,
    C_GRN,
    C_BLU,
    C_MGN,
};
void print_child_scopes(const Vector* scopes) {
    static unsigned int colour_rot_i = 0;

    const char* const col = COLOUR_ROT[colour_rot_i];
    colour_rot_i = (colour_rot_i + 1) % (sizeof (COLOUR_ROT) / sizeof (COLOUR_ROT[0]));

    printf("%s{\n"C_RST, col);
    for (uint i = 0; i < scopes->pos; ++i) {
        const Scope* scope = scopes->arr[i];

        print_scope(scope);
    }
    printf("%s}"C_RST, col);
}

void print_scope(const Scope* scope) {
    const bool is_global = scope->parent == NULL;

    printf("%sScope: \n"
           "Brace Node: ",
           is_global ? "Global " : ""
    );

    print_token(scope->scope ? scope->scope->token : NULL);

    printf("\nCreation Node: ");
    print_token(scope->creation_node ? scope->creation_node->token : NULL);
    putz(" Peek(");
    putz_santitize(scope->creation_node ? (char*)plines->arr[scope->creation_node->token->pos.start_line - 1] : NULL);
    putz(")");

    printf("\nParent Scope: ");
    if (!scope->parent) putz("<>");
    else if (!scope->parent->parent) putz(" Global Scope");
    else if (scope->parent->creation_node) print_token(scope->parent->creation_node->token);
    else putz("Unable to determine parent scope type");

    printf("\nVariables:\n");
    print_variables(&scope->variables);

    printf("\nSubroutines:\n");
    print_subroutines(&scope->subroutines);

    printf("\nInner scopes: \n");
    print_child_scopes(&scope->child_scopes);

    printf("\n---SCOPE---END---\n");
}