//
// Created by james on 01/08/24.
//

#include "TPPParser.h"
#include "TPPParserInternal.h"
#include "TypePreprocessorInternal.h"
#include "TPPLexer.h"
#include "../SharedIncludes/Messages.h"

uint t_pos;
Array t_tokens;

const Vector* t_types_enum;

Array type_aliases;
Array operator_types;

static bool expect(TPPType type);
static TPPToken* consume(void);
static TPPToken* peek(void);
static TPPToken* current(void);

static const char* get_type_string(uint32_t position);
static const char* get_tpptoken_type_string_from_token(const TPPToken* token);

static void print_type_bitmap(uint32_t type_vector);

static void add_type(uint32_t type_value, uint32_t* type_vector);
static bool type_exists(uint32_t type_value, uint32_t type_vector);

static TPPNode* tpp_parse_statement(void);
static TPPNode* tpp_parse_alias_stmnt(void);
static TPPNode* tpp_parse_operator_stmnt(void) {}
static TPPNode* tpp_parse_coerce_stmnt(void) {}

TPPNode* tpp_parse(Array tokens, const Vector* types_enums,
                   const Vector* operator_enums) {
    t_pos = 0;
    t_tokens = tokens;
    t_types_enum = types_enums;

    type_aliases = arr_create(sizeof (TypeAlias));
    operator_types = arr_create(sizeof (OpInfo));

    TPPNode* global_node = malloc(sizeof (TPPNode));

    while (current() && current()->type != EOS) {
        TPPNode* statement = tpp_parse_statement();
        vector_add(&global_node->children, statement);
    }

    return global_node;
}

static TPPNode* tpp_parse_statement() {
    if (current()->type == ALIAS) {
        return tpp_parse_alias_stmnt();
    }

    const TPPToken* next = peek();

    if (next->type == EQUALITY) {
        return tpp_parse_operator_stmnt();
    }
    if (next->type == ARROW) {
        return tpp_parse_coerce_stmnt();
    }

    error(
        "Invalid statement start. Expected ALIAS|OPERATOR|COERCE statement, got tokens `%s` and `%s`",
        get_tpptoken_type_string(current()->type),
        next->type
    );

    return NULL;
}

static TPPNode* tpp_parse_alias_stmnt() {
    TPPToken* alias_node = consume();

    assert(alias_node->type == ALIAS);

    if (!expect(IDENTIFIER)) {
        const TPPToken* c = current();
        error("Expected identifier to follow ALIAS keyword got `%s`",
              get_tpptoken_type_string_from_token(c));
        // [[todo]]: need to either return an error node or have some kind of recovery from this point
    }

    // an ALIAS statement is in the format
    //  ALIAS <IDENTIFIER> `=` <TYPE> <TYPE> <TYPE> // [[maybe]] could add the ability to use the aliases within each other; for now it has to be a type

    const TPPToken* identifier = consume();

    if (!expect(EQUALITY)) {
        error("Expected equality sign `=` after alias identifier got `%s`",
              get_tpptoken_type_string_from_token(current()));
    }

    consume(); // eat the `=`

    TypeAlias alias = (TypeAlias) {
        .identifier = identifier->data.str,
        .types = 0
    };

    while (expect(TYPE)) {
        const TPPToken* type = consume();

        const uint32_t type_value = type->data.pos;

        if (type_exists(type_value, alias.types)) {
            error("ALIAS `%s` already includes the type `%s`",
                  alias.identifier,
                  get_type_string(type->data.pos)
            );
        }

        add_type(type_value, &alias.types);

        printf("Added type `%s` to vector with bit map: ",
               get_type_string(type->data.pos)
        );
        print_type_bitmap(alias.types);
    }
}

static void print_type_bitmap(uint32_t type_vector) {
    for (int i = 0; i < 32; ++i) {
        putchar(type_vector >> (32 - i) & 1 ? 'X' : '-');
    }
}

static void add_type(uint32_t type_value, uint32_t* type_vector) {
    *type_vector |= (1 << type_value);
}

static bool type_exists(uint32_t type_value, uint32_t type_vector) {
    return (type_vector & (1 << type_value));
}

static const char* get_type_string(uint32_t position) {
    return t_types_enum->arr[position];
}

static const char* get_tpptoken_type_string_from_token(const TPPToken* token) {
    return !token ? "<<NULL>>" : get_tpptoken_type_string(token->type);
}

static bool expect(TPPType type) {
    return (current() && current()->type == type);
}

static bool is_valid_index(uint index) {
    return index < t_tokens.pos;
}

static TPPToken* consume() {
    if (!is_valid_index(t_pos)) return NULL;

    return arr_get(&t_tokens, t_pos++);
}

static TPPToken* peek() {
    if (!is_valid_index(t_pos + 1)) return NULL;

    return arr_get(&t_tokens, t_pos + 1);
}

static TPPToken* current() {
    if (!is_valid_index(t_pos)) return NULL;

    return arr_get(&t_tokens, t_pos);
}