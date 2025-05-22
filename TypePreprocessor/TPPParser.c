//
// Created by james on 01/08/24.
//

#include "TPPParser.h"

#include <stdlib.h>
#include <string.h>

#include "TPPParserInternal.h"
#include "TypePreprocessor.h"
#include "TPPLexer.h"
#include "SharedIncludes/Helper_Math.h"
#include "SharedIncludes/Messages.h"

const Vector* t_types_enum;

TypeFixInfoArray typefixes;
TypeInfoArray types;
OperatorInfoArray operators;
AliasInfoArray aliases;

TypeMatrix coercions;
OperandInfoArray operands;

static TPPToken* expect(TPPType type);
static TPPToken* expect_keyword(enum KEYWORDS keyword);
static TPPToken* consume(void);
static TPPToken* peek(void);
static TPPToken* current(void);

static int unexpected(const TPPType expected, const char* context, const TPPToken* found);

static const char* get_type_string(uint32_t position);
static const char* get_tpptoken_type_string_from_token(const TPPToken* token);

static void print_type_bitmap(uint32_t type_vector);

static void add_type(uint32_t type_value, uint32_t* type_vector);
static bool type_exists(uint32_t type_value, uint32_t type_vector);

static TPPNode* tpp_parse_statement(const TPPToken* c);

static void parse_keyword_statement(const TPPToken* token);
static void parse_identifier_statement(const TPPToken* identifier_token);

static void print_typefix_info(const TypeFixInfo* info);
static void print_type_info(const TypeInfo* info);
static const char* assoc_str(char assoc) ;
static const char* op_type_str(char op_type);
static void print_operator_info(const OperatorInfo* info);
static void print_typemap_selected(const char* prefix, uint64_t map);
static void print_typemap_all(const char* prefix, uint64_t map);
static void print_alias_info(const AliasInfo* info);
static void print_coercion_info(const CoercionInfo* info);

size_t t_idx= 0;
Array t_tokens;

sections section_state= SECTION_NONE;
sections last_section_state= SECTION_NONE;
DefaultInfo default_info;

const char* section_strings[SECTION_COUNT] = {
    [SECTION_TYPEFIX]= "Typefix",
    [SECTION_TYPES]= "Types",
    [SECTION_OPERATORS]= "Operators",
    [SECTION_ALIASES]= "Aliases",
    [SECTION_COERCIONS]= "Coercions",
    [SECTION_OPERANDS]= "Operands"
};

bool parsed_sections[SECTION_COUNT] = {};

#define MAX_SECTION_REQUIREMENTS 5
int section_requirements[SECTION_COUNT][MAX_SECTION_REQUIREMENTS] = {
    [SECTION_TYPEFIX]= {-1},
    [SECTION_TYPES]= {SECTION_TYPEFIX, -1},
    [SECTION_OPERATORS]= {-1},
    [SECTION_ALIASES]= {SECTION_TYPES, -1},
    [SECTION_COERCIONS]= {SECTION_TYPES, SECTION_ALIASES, -1},
    [SECTION_OPERANDS]= {
        SECTION_TYPES, SECTION_TYPEFIX,
        SECTION_ALIASES, SECTION_OPERATORS, -1
    }
};

void set_section_parsed(const sections section) {
    parsed_sections[section]= true;
}

void print_section_requirements() {
    for (int i = 0; i < SECTION_COUNT; ++i) {
        const sections section= i;
        printf("Section: `%s` requires:\n", section_strings[section]);

        const int* requirements= section_requirements[section];

        if (*requirements == -1) {
            printf("None");
            continue;
        }

        while (*requirements != -1) {
            printf("  %s\n", section_strings[*requirements]);
            requirements++;
        }
    }
}

int assert_required_sections_met(const sections section) {
    for (int i = 0; i < MAX_SECTION_REQUIREMENTS; ++i) {
        const sections requirement= section_requirements[section][i];

        if (requirement == -1) break;

        if (!parsed_sections[requirement]) {
            const int ret_code= error("Section requirement not met section `%s` requires that section `%s` exists before.\n"
                         "Recommended section order is TYPEFIX -> TYPES -> OPERATORS -> ALIASES -> COERCIONS -> OPERANDS.\n"
                         "Section requirements are as follows: \n");
            print_section_requirements();

            return ret_code;
        }
    }

    return EXIT_SUCCESS;
}

void setup_arrays() {
    typefixes= TypeFixInfo_arr_create();
    types=     TypeInfo_arr_create();
    operators= OperatorInfo_arr_create();
}

void parse_types_file(const Array tokens) {
    t_tokens= tokens;

    setup_arrays();

    while (t_idx < t_tokens.pos) {
        const TPPToken* token= current();

        switch (token->type) {
            case KEYWORD:
                parse_keyword_statement(token);
            break;
            case IDENTIFIER:
                parse_identifier_statement(token);
            break;
            default:
                error("Expected statement to start with either a KEYWORD or IDENTIFIER found: %s", get_tpptoken_type_string(token->type));
                consume();
        }
    }
}

static void clean_default() {
    default_info.has_info= false;
}

static void warn_no_default() {
    warning(
        "No default information should be given for the current state: `%d`",
        section_state
    );
}

static void set_default_typefix(const enum KEYWORDS keyword) {
    TypeFixInfo* info= &default_info.info.type_fix;
    switch (keyword) {
        case PREFIX:
            info->prefix= true;
        break;
        default:
            error("Invalid keyword given to default settings for typefix. Got `%s`", get_tpptoken_keyword_string(keyword));
        break;
    }
}

static DefaultOperatorInfoAssoc keyword_to_info_assoc(const enum KEYWORDS keyword) {
    switch (keyword) {
        case LEFT:
            return OIA_LEFT;
        case RIGHT:
            return OIA_RIGHT;
        default:
            assert(false);
    }
}

static DefaultOperatorInfoGType keyword_to_info_gtype(const enum KEYWORDS keyword) {
    switch (keyword) {
        case BI:
            return OIGT_BINARY;
        case PREFIX:
            return OIGT_PREFIX;
        case POSTFIX:
            return OIGT_POSTFIX;
        case TRI:
            return OIGT_TRINARY;
        default:
            assert(false);
    }
}

static void set_default_operators(const enum KEYWORDS keyword) {
    DefaultOperatorsInfo* info= &default_info.info.operators;
    switch (keyword) {
        case LEFT:
        case RIGHT:
            info->assoc= keyword_to_info_assoc(keyword);
            break;
        case PREFIX:
        case POSTFIX:
        case BI:
        case TRI:
            info->general_type= keyword_to_info_gtype(keyword);
            break;
        default:
            error("Invalid keyword given to default settings for operators. Got `%s`", get_tpptoken_keyword_string(keyword));
        break;
    }
}

static void set_default(const enum KEYWORDS keyword) {
    default_info.has_info= true;
    switch(section_state) {
        case SECTION_TYPEFIX:
            set_default_typefix(keyword);
        break;
        case SECTION_OPERATORS:
            set_default_operators(keyword);
        break;
        case SECTION_TYPES:
        case SECTION_ALIASES:
        case SECTION_COERCIONS:
        case SECTION_OPERANDS:
            warn_no_default();
        break;
        default:
            assert(false);
    }
}

void update_state(const enum KEYWORDS keyword) {
    switch (keyword) {
        case TYPEFIX: section_state= SECTION_TYPEFIX; break;
        case TYPES: section_state= SECTION_TYPES; break;
        case OPERATORS: section_state= SECTION_OPERATORS; break;
        case ALIASES: section_state= SECTION_ALIASES; break;
        case COERCIONS: section_state= SECTION_COERCIONS; break;
        case OPERANDS: section_state= SECTION_OPERANDS; break;
        default:
            assert(false);
    }
}

bool is_header_keyword(enum KEYWORDS keyword) {
    switch (keyword) {
        case TYPEFIX:
        case TYPES:
        case OPERATORS:
        case ALIASES:
        case COERCIONS:
        case OPERANDS:
            return true;
        default:
            return false;
    }
}

void cleanup_section(sections section) {
    switch (section) {
        case SECTION_NONE:
            return;

        case SECTION_TYPEFIX:
            TypeFixInfo_arr_sort_i(&typefixes);
            break;

        case SECTION_TYPES:
            TypeInfo_arr_sort_i(&types);
    }
}

void parse_keyword_statement(const TPPToken* token) {
    if (last_section_state != SECTION_NONE) {
        cleanup_section(last_section_state);
    }
    last_section_state= section_state;

    const enum KEYWORDS keyword = token->data.keyword;

    if (!is_header_keyword(keyword)) {
        error("Expected keyword statement to start with a valid header keyword i.e. TYPEFIX, TYPES, ALIASES, OPERATORS, COERCIONS, OPERANDS. Got %s", get_tpptoken_keyword_string(keyword));
        return;
    }

    update_state(keyword);

    if (assert_required_sections_met(section_state) != EXIT_SUCCESS) {
        return;
    }

    clean_default();
    if (!expect_keyword(DEFAULT)) return;

    TPPToken* tok;
    while (tok= expect(KEYWORD), tok) {
        set_default(tok->data.keyword);
    }

    if (!expect(EOS)) {
        error("Unexpected token type in keyword statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
    }

    set_section_parsed(section_state);
}

bool is_symbolic(const TPPType type) {
    switch (type) {
        case EQUALITY:
        case ARROW:
        case BIRROW:
        case PIPE:
        case AND:
        case OR:
        case DELIMITER:
            return true;
        default:
            return false;
    }
}

const char* op_type_to_str[] = {
    [EQUALITY]= "=",
    [DELIMITER]= ";",
    [OR]= "||",
    [PIPE]= "|",
    [AND]= "&&",
    [BIRROW]= "<->",
    [ARROW]= "->"
};

#define MAX_OP_TYPE_LEN 4

char* to_custom_op(const TPPType type) {
    char* ret= malloc(MAX_OP_TYPE_LEN);

    if (!is_symbolic(type)) return NULL;

    const char* copy= op_type_to_str[type];

    memcpy(ret, copy, strlen(copy));

    return ret;
}

char* parse_symbol() {
    // a custom symbol may be the same as a symbol used by the pre-proc
    // so convert any symbols to customs operators
    const TPPToken* c= current();

    char* symbol= to_custom_op(c->type);

    if (!symbol) {
        error("Unable to convert non-symbolic types to custom operator. Found: `%s`", get_tpptoken_type_string_from_token(c));
        return NULL;
    }

    consume();

    return symbol;
}

int parse_typefix_identifier_statement() {
    // The typefix line consists of
    //  <name> <symbol> <PREFIX|POSTFIX>? <EOS>
    const TPPToken* identifier= consume();
    const char* symbol= parse_symbol();

    if (!symbol) {
        return error("Unable to parse typefix statement from invalid symbol");
    }

    bool prefix= false;

    if (default_info.has_info) {
        prefix= default_info.info.type_fix.prefix;
    }

    if (expect_keyword(PREFIX)) prefix= true;
    if (expect_keyword(POSTFIX)) prefix= false;

    if (!expect(EOS)) {
        return error("Typefix statement has postceding non-EOS tokens. Found `%s`", get_tpptoken_type_string_from_token(peek()));
    }

    const TypeFixInfo info = (TypeFixInfo) {
        .name= identifier->data.str,
        .symbol= symbol,
        .prefix= prefix ? 1 : 0
    };

    TypeFixInfo_arr_add(&typefixes, info);

    print_typefix_info(&info);

    return EXIT_SUCCESS;
}

uint find_typefix(const char* name) {
    return TypeFixInfo_arr_search_i(&typefixes, name);
}

uint find_type(const char* name) {
    return TypeInfo_arr_search_i(&types, name);
}

uint find_alias(const char* name) {
    return AliasInfo_arr_search_i(&aliases, name);
}

struct TypeLikePos {
    enum {
        TYPE_TYPE,
        TYPE_TYPEFIX,
        TYPE_ALIAS
    } type;
    uint pos;
} find_type_like(const char* name) {
    const uint typefix_pos= find_typefix(name);
    const uint type_pos= find_type(name);
    const uint alias_pos= find_alias(name);

    if (typefix_pos == -1 && type_pos == -1 && alias_pos == -1)
        return {.type= TYPE_TYPE, .pos= -1};

    if (type_pos != -1)
        return {.type= TYPE_TYPE, .pos= type_pos};

    if (typefix_pos != -1)
        return {.type= TYPE_TYPEFIX, .pos= typefix_pos};

    return {.type= TYPE_ALIAS, .pos= alias_pos};
}

int parse_types_tail(TypeInfo* info) {
    TPPToken* p= expect(KEYWORD);

    if (!p) {
        return error("Expected keyword as first token in tail of types statement. Found `%s`", get_tpptoken_type_string_from_token(p));
    }

    const enum KEYWORDS keyword= p->data.keyword;

    if (keyword == VIRTUAL) {
        info->is_virtual= true;
    } else if (keyword == REQUIRE) {
        uintArray requirements= uint_arr_create();

        TPPToken* requirement= expect(IDENTIFIER);

        if (!requirement) {
            return error("Expected typefix requirement after require keyword in types statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
        }

        while (requirement) {
            const uint pos= find_typefix(requirement->data.str);

            if (pos == (uint)-1) {
                uint_arr_destroy(&requirements);

                return error("Type requirement must be a typefix element. Found requirement `%s`\n", requirement->data.str);
            }

            uint_arr_add(&requirements, pos);

            requirement= expect(IDENTIFIER);
        }

        info->requirements= requirements;
    } else {
        return error("Expected only VIRTUAL or REQUIRE keyword in type info tail. Found `%s`", get_tpptoken_type_string_from_token(p));
    }
}

int parse_types_identifier_statement() {
    // The type line consists of
    //  <general name> PREFIX <prefix> OVER <SIZE..>
    // OR
    //  <general name> <name>
    // WITH TRAILING
    //  VIRTUAL? REQUIRE <REF: TYPEFIX..>?

    const TPPToken* general_ident= consume();

    TypeInfo info = (TypeInfo) {
        .general_type= general_ident->data.str
    };

    if (default_info.has_info) {}

    if (expect_keyword(PREFIX)) {
        // <prefix> OVER <SIZE..>
        const TPPToken* prefix= expect(IDENTIFIER);

        if (!prefix) {
            return error("Expected identifier after prefix keyword in types statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
        }

        if (!expect_keyword(OVER)) {
            return error("Expected keyword OVER after prefix identifier in types statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
        }

        uintArray sizes = uint_arr_create();
        TPPToken* size;
        while (size= expect(NUMERIC), size) {
            // [[todo]] this is long long -> uint
            //  but it shouldn't really matter for the
            //  sizes that you'd actually use
            //  it is still a bug
            uint_arr_add(&sizes, size->data.numeric);
        }

        if (sizes.pos == 0) {
            return error("Expected at least one size after OVER keyword in types statement. Found None before type: `%s`", get_tpptoken_type_string_from_token(peek()));
        }

        info.has_variable_sizes= true;
        info.prefix= prefix->data.str;
        info.sizes= sizes;
    } else {
        // <name>
        const TPPToken* identifier= expect(IDENTIFIER);

        if (!identifier) {
            return error("Expected identifier after general type in type statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
        }

        info.name= identifier->data.str;
    }

    TPPToken* c;
    while (c= current(), c && c->type != EOS) {
        parse_types_tail(&info);
    }

    TypeInfo_arr_add(&types, info);

    print_type_info(&info);

    return EXIT_SUCCESS;
}

int parse_operator_tail(OperatorInfo* info) {
    const TPPToken* c;
    if (c=expect(KEYWORD), !c) {
        return error(
            "Expected keyword token in tail of operator statement. Found `%s`\n",
            get_tpptoken_type_string_from_token(peek())
        );
    }

    const enum KEYWORDS keyword= c->data.keyword;
    switch (keyword) {
        case PREFIX:
        case POSTFIX:
        case BI:
        case TRI:
            info->op_type= keyword_to_info_gtype(keyword);
            break;
        case LEFT:
        case RIGHT:
            info->assoc= keyword_to_info_assoc(keyword);
            break;
        default:
            return error(
                "Expected assoc (LEFT, RIGHT) or general type (BI, TRI, PREFIX, POSTFIX) keyword. Found `%s`\n",
                get_tpptoken_keyword_string(keyword)
            );
    }

    return EXIT_SUCCESS;
}

int parse_operators_identifier_statement() {
    const DefaultOperatorsInfo* default_op= &default_info.info.operators;

    const TPPToken* identifier= consume();

    const char* symbol= parse_symbol();

    if (!symbol) {
        return error("Expected valid symbol after identifier in operator statement");
    }

    const TPPToken* pres= expect(NUMERIC);

    if (!pres) {
        return error("Expected numeric precedence after symbol in operator statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
    }

    OperatorInfo info= (OperatorInfo){
        .name= identifier->data.str,
        .symbol= symbol,
    };

    if (default_info.has_info) {
        info.assoc= default_op->assoc;
        info.op_type= default_op->general_type;
    }

    TPPToken* c;
    while (c= current(), c && c->type != EOS) {
        parse_operator_tail(&info);
    }

    OperatorInfo_arr_add(&operators, info);

    return EXIT_SUCCESS;
}

int add_to_type_map(uint64_t* map, const char* name) {
    const struct TypeLikePos res= find_type_like(name);

    if (res.pos == -1) return EXIT_FAILURE;

    switch (res.type) {
        case TYPE_TYPE:
            *map |= res.pos;
        break;
        case TYPE_TYPEFIX:
            *map |= res.pos + types.pos;
        break;
        case TYPE_ALIAS:
            const AliasInfo* alias= AliasInfo_arr_ptr(&aliases, res.pos);
        *map |= alias->type_map;
        break;
    }

    return EXIT_SUCCESS;
}

int parse_type_pipe_statement(uint64_t* type_map) {
    // format type_like | type_like | ...
    while (true) {
        const TPPToken* type_ident= expect(IDENTIFIER);

        if (!type_ident) {
            return unexpected(IDENTIFIER, "Type pipe statement", peek());
        }

        if (!add_to_type_map(type_map, type_ident->data.str)) {
            return error("Could not find type identifier in types, typefixes, or aliases. Found identifier `%s`", identifier->data.str);
        }

        if (!expect(PIPE)) {
            break;
        }
    }

    return EXIT_SUCCESS;
}

int parse_aliases_identifier_statement() {
    const TPPToken* identifier= consume();

    if (!expect(EQUALITY)) {
        return unexpected(EQUALITY, "Alias statement", peek());
    }

    AliasInfo info= {.name= identifier->data.str, .type_map= 0};

    parse_type_pipe_statement(&info.type_map);

    if (!expect(EOS)) {
        return unexpected(EOS, "End of valid aliases in Alias statement", peek());
    }

    AliasInfo_arr_add(&aliases, info);

    return EXIT_SUCCESS;
}

int parse_coercions_identifier_statement() {
    uint64_t left_type_map= 0;
    uint64_t right_type_map= 0;

    parse_type_pipe_statement(&left_type_map);

    bool is_bidirectional= false;
    if (is_bidirectional= expect(BIRROW), !is_bidirectional && !expect(ARROW)) {
        return unexpected(BIRROW, "Coercion statement", peek());
    }

    parse_type_pipe_statement(&right_type_map);

parse_coercions_error:
    return EXIT_FAILURE;
}

int parse_operands_identifier_statement(const TPPToken* identifier_tok) {

}

void parse_identifier_statement(const TPPToken* identifier_token) {
    int code;
    switch (section_state) {
        case SECTION_TYPEFIX:
            code= parse_typefix_identifier_statement();
            break;
        case SECTION_TYPES:
            code= parse_types_identifier_statement();
            break;
        case SECTION_OPERATORS:
            code= parse_operators_identifier_statement(identifier_token);
            break;
        case SECTION_ALIASES:
            code= parse_aliases_identifier_statement(identifier_token);
            break;
        case SECTION_COERCIONS:
            code= parse_coercions_identifier_statement(identifier_token);
            break;
        case SECTION_OPERANDS:
            code= parse_operands_identifier_statement(identifier_token);
            break;
        case SECTION_NONE:
            code= error("Expected state header e.g. ALIASES, TYPEFIX, etc before identifier statement. Got identifier `%s`", identifier_token->data.str);
            break;
        default:
            assert(false);
    }

    if (code != EXIT_SUCCESS) {
        // if we did not successfully parse a line
        //  then just go to the end of the line to skip bad tokens
        TPPToken* c;
        while (c= current(), c && c->type != EOS) consume();
        consume();
    }
}

static void add_type(uint32_t type_value, uint32_t* type_vector) {
    *type_vector |= (1 << type_value);
}

static bool type_exists(uint32_t type_value, uint32_t type_vector) {
    return (type_vector & (1 << type_value));
}

static const char* get_type_string(const uint32_t position) {
    return t_types_enum->arr[position];
}

static const char* get_tpptoken_type_string_from_token(const TPPToken* token) {
    return !token ? "<<NULL>>" : get_tpptoken_type_string(token->type);
}

void print_typefix_info(const TypeFixInfo* info) {
    printf("Typfix:\n"
           "  Name   : `%s`\n"
           "  Symbol : `%s`\n"
           "  Prefix?: %s\n\n",
           info->name,
           info->symbol,
           info->prefix ? "True" : "False"
    );
}

void print_type_info(const TypeInfo* info) {
    printf("Type (%s):\n"
           "  General: `%s`\n",
           info->has_variable_sizes ? "Variable sized" : "Fixed size",
           info->general_type
    );

    if (info->has_variable_sizes) {
        printf("  Prefix : `%s`\n"
               "  sizes  :\n",
               info->prefix
        );
        for (int i = 0; i < info->sizes.pos; ++i) {
            const uint size= uint_arr_get(&info->sizes, i);

            printf("    - %u\n", size);
        }
    } else {
        printf("  Name   : `%s`\n", info->name);
    }

    printf("  Virtual?: %s\n"
           "  Requirements: %s\n",
           info->is_virtual ? "True" : "False",
           info->requirements.pos == 0 ? "None" : ""
    );

    for (int i = 0; i < info->requirements.pos; ++i) {
        const uint req_pos= uint_arr_get(&info->requirements, i);
        const TypeFixInfo* req= TypeFixInfo_arr_ptr(&typefixes, req_pos);

        printf("    - `%s`\n", req->name);
    }

    newline();
}

// [[todo]] conv to []
const char* assoc_str(const char assoc) {
    switch (assoc) {
        case OIA_LEFT : return "LEFT";
        case OIA_RIGHT: return "RIGHT";
        default: assert(false);
    }
}

const char* op_type_str(const char op_type) {
    switch (op_type) {
        case OIGT_BINARY: return "BINARY";
        case OIGT_POSTFIX: return "POSTFIX";
        case OIGT_PREFIX: return "PREFIX";
        case OIGT_TRINARY: return "TRINARY";
    }
}

void print_operator_info(const OperatorInfo* info) {
    printf("Operator:\n"
           "  Name      : `%s`\n"
           "  Symbol    : `%s`\n"
           "  Precedence: %u\n"
           "  Assoc     : %s\n"
           "  Op Type   : %s\n",
           info->name,
           info->symbol,
           info->precedence,
           assoc_str(info->assoc),
           op_type_str(info->op_type)
    );
}

_Static_assert(sizeof(uint64_t) * 8 == 64);
void print_typemap_selected(const char* prefix, const uint64_t map) {
    assert(types.pos + typefixes.pos <= 64);
    // - - - - - 0 1 0 0 1 1 1
    // ^-------^ ^-^ ^-------^
    //  EXCESS  TYFIX  TYPES
    const uint typefix_start= types.pos + typefixes.pos - 1;
    for (uint i= 0; i < typefixes.pos; i++) {
        const uint index= typefix_start - i;
        if (map >> index & 1) {
            const TypeFixInfo* typefix= TypeFixInfo_arr_ptr(&typefixes, i);
            printf("%s%s (%s: %s)\n",
                prefix,
                typefix->name,
                typefix->symbol,
                typefix->prefix ? "PREFIX" : "POSTFIX"
            );
        }
    }
    for (uint i = 0; i < types.pos; ++i) {
        const uint index= types.pos - 1 - i;
        if (map >> index & 1) {
            const TypeInfo* type= TypeInfo_arr_ptr(&types, i);
            printf("%s%s (%s)\n",
                prefix,
                type->general_type,
                type->has_variable_sizes ? type->prefix : type->name
            );
        }
    }
}

void print_typemap_all(const char* prefix, const uint64_t map) {
    assert(types.pos + typefixes.pos <= 64);

    const uint typefix_start= types.pos + typefixes.pos - 1;
    for (uint i= 0; i < typefixes.pos; i++) {
        const uint index= typefix_start - i;
        const TypeFixInfo* typefix= TypeFixInfo_arr_ptr(&typefixes, i);
        const bool active= map >> index & 1;
        printf("%s%s: %s (%s: %s)\n",
            prefix,
            typefix->name,
            active ? "X" : "-",
            typefix->symbol,
            typefix->prefix ? "PREFIX" : "POSTFIX"
        );
    }
    for (uint i= 0; i < types.pos; ++i) {
        const uint index= types.pos - 1 - i;
        const bool active= map >> index & 1;

        const TypeInfo* type= TypeInfo_arr_ptr(&types, i);
        printf("%s%s: %s (%s)\n",
            prefix,
            type->general_type,
            active ? "X" : "-",
            type->has_variable_sizes ? type->prefix : type->name
        );
    }
}

void print_alias_info(const AliasInfo* info) {
    printf("Alias: \n"
           "  Name    : `%s`\n"
           "  Type map:\n",
           info->name);

    print_typemap_all("    - ", info->type_map);

    newline();
}

void print_type_matrix(const TypeMatrix matrix) {
    assert(types.arr);

    const uint type_c= types.pos;

    /*      0  1  2 ... 7  8  9 (type_c)
     *    |-----------------------------
     *  0 |
     *  1 |
     *  2 |
     *  ...
     *  7 |      NP
     *  8 |
     *  9 |
     *
     *  NP= NEW PAD START (9 * 7 + 2= 65)
     */

    putz("      ");
    for (uint i = 0; i < type_c; ++i) {
        printf("%u  ", i);
    }
    newline();

    putz("    |");
    for (uint i = 0; i < type_c; ++i) {
        putz("---");
    }
    newline();

    _Static_assert(sizeof(uint64_t) * 8 == 64);
    for (uint i = 0; i < type_c; ++i) {
        printf("  %u |", i);
        for (uint j = 0; j < type_c; ++j) {
            uint index= i * type_c + j;
            // [[todo]] this can be changed into a top and bottom calculation
            //  as we know that the first 64 are top pad
            //  so this can be two loops
            const bool bottom_pad= index > 64;

            bool marked= false;
            if (bottom_pad) {
                index -= 64;
                marked= matrix.bottom_pad >> index & 1;
            } else {
                marked= matrix.top_pad >> index & 1;
            }

            printf(" %c ", marked ? 'X' : ' ');
        }
        newline();
    }
}

void print_coercion_info(const CoercionInfo* info) {
    printf("Coercions: \n");
    print_type_matrix(info->matrix);

    newline();
}

static TPPToken* expect(const TPPType type) {
    if (peek()->type == type) {
        return consume();
    }

    return NULL;
}

static TPPToken* expect_keyword(const enum KEYWORDS keyword) {
    if (peek()->type == KEYWORD && peek()->data.keyword == keyword) {
        return consume();
    }

    return NULL;
}

static bool is_valid_index(uint index) {
    return index < t_tokens.pos;
}

static TPPToken* consume() {
    if (!is_valid_index(t_idx)) return NULL;

    return arr_ptr(&t_tokens, t_idx++);
}

static TPPToken* peek() {
    if (!is_valid_index(t_idx + 1)) return NULL;

    return arr_ptr(&t_tokens, t_idx + 1);
}

static TPPToken* current() {
    if (!is_valid_index(t_idx)) return NULL;

    return arr_ptr(&t_tokens, t_idx);
}

static int unexpected(const TPPType expected, const char* context, const TPPToken* found) {
    printf(
        "Expected `%s` in %s. Found `%s`",
        get_tpptoken_type_string(expected),
        context,
        get_tpptoken_type_string_from_token(found)
    );

    return EXIT_FAILURE;
}
