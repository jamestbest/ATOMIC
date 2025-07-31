//
// Created by james on 01/08/24.
//

#include "TPPParser.h"

#include "SharedIncludes/Helper_Math.h"
#include "SharedIncludes/Messages.h"
#include "TPPLexer.h"
#include "TPPParserInternal.h"
#include "TypePreprocessor.h"

#include <Errors.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

const Vector* t_types_enum;

static TPPToken* expect(TPPType type);
static TPPToken* expect_keyword(enum KEYWORDS keyword);
static TPPToken* consume(void);
static TPPToken* peek(void);
static TPPToken* current(void);

static errcode unexpected(const TPPType
                              expected,
                          const char*
                              context);

static const char* get_type_string(uint32_t position);
static const char* get_tpptoken_type_string_from_token(const TPPToken* token);

static void print_type_bitmap(uint32_t type_vector);

static void add_type(uint32_t type_value, uint32_t* type_vector);
static bool type_exists(uint32_t type_value, uint32_t type_vector);

static void add_to_type_matrix(TypeMatrix matrix, uint32_t x, uint32_t y);
static void or_type_matrices(TypeMatrix dst, TypeMatrix or);

static TPPNode* tpp_parse_statement(const TPPToken* c);

static char* parse_symbol();

static errcode parse_keyword_statement(const TPPToken* token);
static errcode parse_identifier_statement(const TPPToken* identifier_token);

static void enter_section(sections section);

static void print_typefix_info(const TypeFixInfo* info);
static void print_type_info(const TypeInfo* info);
static const char* assoc_str(unsigned char assoc);
static const char* op_type_str(unsigned char op_type);
static void print_operator_info(const OperatorInfo* info);
static void print_typemap_selected(const char* prefix, uint64_t map);
static void print_typemap_all(const char* prefix, uint64_t map);
static void print_alias_info(const AliasInfo* info);
static void print_coercion_info(const CoercionInfo* info);
static void print_operand_info(const OperandInfo* info);
static void print_type_matrix(const char* prefix, TypeMatrix matrix);
static void print_type_matrix_diff(TypeMatrix old, TypeMatrix new);

ARRAY_JOINT(char*, String)

size_t t_idx= 0;
TPPTokenArray t_tokens;;

sections section_state= SECTION_NONE;
sections last_section_state= SECTION_NONE;
DefaultInfo default_info;

const char* section_strings[SECTION_COUNT] = {
    [SECTION_NONE]= "None",
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
    [SECTION_NONE]= {-1},
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
            printf("  None\n");
            continue;
        }

        while (*requirements != -1) {
            printf("  %s\n", section_strings[*requirements]);
            requirements++;
        }
    }
}

errcode assert_required_sections_met(const sections section) {
    for (int i = 0; i < MAX_SECTION_REQUIREMENTS; ++i) {
        const sections requirement= section_requirements[section][i];

        if (requirement == (uint)-1) break;

        if (!parsed_sections[requirement]) {
            const errcode ret_code= error("Section requirement not met section `%s` requires that section `%s` exists before.\n"
                         "Recommended section order is TYPEFIX -> TYPES -> OPERATORS -> ALIASES -> COERCIONS -> OPERANDS.\n"
                         "Section requirements are as follows: \n", section_strings[section], section_strings[requirement]);
            print_section_requirements();

            return ret_code;
        }
    }

    return SUCC;
}

static TypeMatrix create_type_matrix();

void setup_arrays() {
    typefixes= TypeFixInfo_arr_create();
    types=     TypeInfo_arr_create();
    operators= OperatorInfo_arr_create();
    aliases=   AliasInfo_arr_create();
    operands=  OperandInfo_arr_create();
}

errcode parse_types_file(const TPPTokenArray tokens) {
    t_tokens= tokens;

    setup_arrays();

    errcode code= SUCC;
    while (t_idx < t_tokens.pos) {
        const TPPToken* token= current();

        errcode stmt_code;
        switch (token->type) {
            case KEYWORD:
                stmt_code= parse_keyword_statement(token);
                break;
            case IDENTIFIER:
                stmt_code= parse_identifier_statement(token);
                break;
            default:
                stmt_code= error("Expected statement to start with either a KEYWORD or IDENTIFIER found: %s\n", get_tpptoken_type_string(token->type));
                consume();
        }

        if (stmt_code.code != SUCCESS) {
            if (stmt_code.fatal) {
                return fatal("TPPParser encountered fatal error during parsing. Terminating `%s`", error_code_string(stmt_code));
            }
            code= stmt_code;
        }
    }

    return code;
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
    DefaultTypeFixInfo* info= &default_info.info.type_fix;
    switch (keyword) {
        case PREFIX:
            info->prefix= true;
            break;
        default:
            error("Invalid keyword given to default settings for typefix. Got `%s`\n", get_tpptoken_keyword_string(keyword));
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
        case UNARY:
            return OIGT_UNARY;
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
            error("Invalid keyword given to default settings for operators. Got `%s`\n", get_tpptoken_keyword_string(keyword));
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
    enter_section(section_state);
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

void enter_section(sections section) {
    switch (section) {
        case SECTION_COERCIONS:
            coercions= create_type_matrix();
            break;
        default:
            break;
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
            break;

        case SECTION_OPERATORS:
            OperatorInfo_arr_sort_i(&operators);
            break;

        case SECTION_ALIASES:
            AliasInfo_arr_sort_i(&aliases);
            break;
    }
}

errcode parse_keyword_statement(const TPPToken* token) {
    if (section_state != SECTION_NONE) {
        cleanup_section(section_state);
    }

    const enum KEYWORDS keyword = token->data.keyword;

    if (!is_header_keyword(keyword)) {
        return error("Expected keyword statement to start with a valid header keyword i.e. TYPEFIX, TYPES, ALIASES, OPERATORS, COERCIONS, OPERANDS. Got %s", get_tpptoken_keyword_string(keyword));
    }

    update_state(keyword);

    errcode err= assert_required_sections_met(section_state);
    if (err.code != SUCCESS) {
        consume();
        return err;
    }

    consume(); // EAT THE KEYWORD w(ﾟДﾟ)w

    clean_default();
    if (!expect_keyword(DEFAULT)) goto parse_keyword_statement_end;

    TPPToken* tok;
    while (tok=expect(KEYWORD), tok) {
        set_default(tok->data.keyword);
    }

parse_keyword_statement_end:
    if (!expect(EOS)) {
        return error("Unexpected token type in keyword statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
    }

    set_section_parsed(section_state);

    return SUCC;
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

struct SymRet {
    char* sym;
    bool is_ident;
} parse_symbol_or_ident() {
    char* sym= parse_symbol();

    if (sym) return (struct SymRet){.sym= sym, .is_ident= false};

    TPPToken* ident= expect(IDENTIFIER);
    if (!ident) return (struct SymRet){.sym= NULL, .is_ident= false};

    return (struct SymRet){.sym= ident->data.str, .is_ident= true};
}

char* parse_symbol() {
    // a custom symbol may be the same as a symbol used by the pre-proc
    // so convert any symbols to customs operators
    const TPPToken* c= current();

    char* symbol;
    if (c->type == CUSTOM_OPERATOR) symbol= c->data.str;
    else symbol= to_custom_op(c->type);

    if (!symbol) {
        return NULL;
    }

    consume();

    return symbol;
}

errcode parse_typefix_identifier_statement() {
    // The typefix line consists of
    //  <name> <alt-name> <symbol> <PREFIX|POSTFIX>? <EOS>
    const TPPToken* identifier= consume();

    const TPPToken* alt_name_ident= expect(IDENTIFIER);
    if (!alt_name_ident) {
        return unexpected(IDENTIFIER, "alt name of typefix statement");
    }

    char* symbol= parse_symbol();
    if (!symbol) {
        return error("Unable to convert non-symbolic types to custom operator. Found: `%s`\n", get_tpptoken_type_string_from_token(current()));
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
        .alt_name= alt_name_ident->data.str,
        .symbol= symbol,
        .prefix= prefix
    };

    TypeFixInfo_arr_add(&typefixes, info);

    print_typefix_info(&info);

    return SUCC;
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

static size_t type_map_size() {
    return types.pos + typefixes.pos;
}

static size_t type_matrix_size() {
    return type_map_size() * type_map_size();
}

static size_t type_matrix_bytes() {
    return ceil((double)type_matrix_size() / (sizeof (uint8_t) * 8));
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

    const uint MINUS_ONE= (uint)-1;
    if (typefix_pos == MINUS_ONE && type_pos == MINUS_ONE && alias_pos == MINUS_ONE)
        return (struct TypeLikePos){.type= TYPE_TYPE, .pos= -1};

    if (type_pos != MINUS_ONE)
        return (struct TypeLikePos){.type= TYPE_TYPE, .pos= type_pos};

    if (typefix_pos != MINUS_ONE)
        return (struct TypeLikePos){.type= TYPE_TYPEFIX, .pos= typefix_pos};

    return (struct TypeLikePos){.type= TYPE_ALIAS, .pos= alias_pos};
}

errcode parse_types_tail(TypeInfo* info) {
    TPPToken* p= expect(KEYWORD);

    if (!p) {
        return error("Expected keyword as first token in tail of types statement. Found `%s`\n", get_tpptoken_type_string_from_token(p));
    }

    const enum KEYWORDS keyword= p->data.keyword;

    if (keyword == VIRTUAL) {
        info->is_virtual= true;
    } else if (keyword == REQUIRE) {
        uintArray requirements= uint_arr_create();

        TPPToken* requirement= expect(IDENTIFIER);

        if (!requirement) {
            return error("Expected typefix requirement after require keyword in types statement. Found `%s`\n", get_tpptoken_type_string_from_token(peek()));
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
        return error("Expected only VIRTUAL or REQUIRE keyword in type info tail. Found `%s`\n", get_tpptoken_type_string_from_token(p));
    }

    return SUCC;
}

errcode parse_types_identifier_statement() {
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
        // <name> or <name> | <name>
        const TPPToken* identifier;
        info.names= vector_create(4);
        do {
            identifier= expect(IDENTIFIER);

            if (!identifier) {
                return error("Expected identifier after general type in type statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
            }

            vector_add(&info.names, identifier->data.str);
        } while (expect(PIPE));

        if (info.names.pos == 1) {
            info.has_multiple_names= false;
            info.name= identifier->data.str;
            vector_destroy(&info.names);
        } else {
            info.has_multiple_names= true;
        }
    }

    TPPToken* c;
    while (c= current(), c && c->type != EOS) {
        parse_types_tail(&info);
    }

    if (!expect(EOS)) // EAT THE EOS
         return unexpected(EOS, "end of types identifier statement");

    TypeInfo_arr_add(&types, info);

    print_type_info(&info);

    return SUCC;
}

errcode parse_operator_tail(OperatorInfo* info) {
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
        case UNARY:
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

    return SUCC;
}

errcode parse_operators_identifier_statement() {
    const DefaultOperatorsInfo* default_op= &default_info.info.operators;

    const TPPToken* identifier= consume();

    const struct SymRet symbol= parse_symbol_or_ident();

    if (!symbol.sym) {
        return error("Expected valid symbol after identifier in operator statement\n");
    }

    const TPPToken* pres= expect(NUMERIC);

    if (!pres) {
        return error("Expected numeric precedence after symbol in operator statement. Found `%s`", get_tpptoken_type_string_from_token(peek()));
    }

    OperatorInfo info= (OperatorInfo){
        .name= identifier->data.str,
        .symbol= symbol.sym,
        .symbol_is_ident= symbol.is_ident,
        .precedence= pres->data.numeric
    };

    if (default_info.has_info) {
        info.assoc= default_op->assoc;
        info.op_type= default_op->general_type;
    }

    TPPToken* c;
    while (c= current(), c && c->type != EOS) {
        parse_operator_tail(&info);
    }

    if (!expect(EOS)) {
        return unexpected(EOS, "End of valid operator tail in Operator statement");
    }

    OperatorInfo_arr_add(&operators, info);

    print_operator_info(&info);

    return SUCC;
}

int add_to_type_map(uint64_t* map, const char* name) {
    const struct TypeLikePos res= find_type_like(name);

    if (res.pos == (uint)-1) return FAIL;

    switch (res.type) {
        case TYPE_TYPEFIX:
            *map |= 1 << res.pos;
            break;
        case TYPE_TYPE:
            *map |= 1 << (res.pos + typefixes.pos);
            break;
        case TYPE_ALIAS:;
            const AliasInfo* alias= AliasInfo_arr_ptr(&aliases, res.pos);
            *map |= alias->type_map;
            break;
    }

    return SUCCESS;
}

errcode parse_type_pipe_statement(uint64_t* type_map) {
    // format type_like | type_like | ...
    do {
        const TPPToken* type_ident= expect(IDENTIFIER);

        if (!type_ident) {
            return unexpected(IDENTIFIER, "Type pipe statement");
        }

        if (add_to_type_map(type_map, type_ident->data.str) != SUCCESS) {
            return error("Could not find type identifier in types, typefixes, or aliases. Found identifier `%s`\n", type_ident->data.str);
        }
    } while (expect(PIPE));

    return SUCC;
}

errcode parse_aliases_identifier_statement() {
    const TPPToken* identifier= consume();

    if (!expect(EQUALITY)) {
        return unexpected(EQUALITY, "Alias statement");
    }

    AliasInfo info= {
        .name= identifier->data.str,
        .type_map= 0
    };

    parse_type_pipe_statement(&info.type_map);

    if (!expect(EOS)) {
        return unexpected(EOS, "End of valid aliases in Alias statement");
    }

    AliasInfo_arr_add(&aliases, info);

    print_alias_info(&info);

    return SUCC;
}

void type_maps_to_matrix(TypeMatrix matrix, uint64_t left, uint64_t right, bool bi) {
    size_t l_i= 0, r_i=0;
    while (left) {
        uint64_t r_copy= right;
        r_i= 0;
        while (r_copy) {
            if (!left) break;

            while ((left & 1) == 0) {
                left >>= 1;
                l_i++;
            }

            while ((r_copy & 1) == 0) {
                r_copy >>= 1;
                r_i++;
            }

            add_to_type_matrix(matrix, l_i, r_i);
            if (bi)
                add_to_type_matrix(matrix, r_i, l_i);

            r_copy >>= 1;
            r_i++;
        }

        left >>= 1;
        l_i++;
    }
}

errcode parse_coercions_identifier_statement() {
    uint64_t left_type_map= 0;
    uint64_t right_type_map= 0;

    parse_type_pipe_statement(&left_type_map);

    bool is_bidirectional= false;
    if (is_bidirectional= expect(BIRROW), !is_bidirectional && !expect(ARROW)) {
        return unexpected(BIRROW, "Coercion statement");
    }

    parse_type_pipe_statement(&right_type_map);

    TypeMatrix matrix= create_type_matrix();

    type_maps_to_matrix(matrix, left_type_map, right_type_map, is_bidirectional);

    CoercionInfo info= (CoercionInfo){
        .matrix= matrix
    };

    if (!expect(EOS)) {
        return unexpected(EOS, "Coercion statement");
    }

    print_type_matrix_diff(coercions, matrix);
    or_type_matrices(coercions, info.matrix);

    return SUCC;
}

void add_to_op_info(OperandInfo* operand, OperatorInfo* operator, uint64_t l, uint64_t r) {
    if (operator->op_type == OIGT_BINARY) {
        add_to_type_matrix(operand->matrix, l, r);
    } else if (operator->op_type == OIGT_TRINARY) {
        assert(false);
    } else {
        // unary
        operand->typemap |= 1 << l;
    }
}

errcode parse_operands_identifier_statement() {
    TPPToken* op_t;
    if (op_t= expect(IDENTIFIER), !op_t) {
        return unexpected(IDENTIFIER, "Operand statement start");
    }

    uint op_i= OperatorInfo_arr_search_i(&operators, op_t->data.str);
    if (op_i == (uint)-1) {
        return error("Operator `%s` in operands statement has not been defined in operator list\n", op_t->data.str);
    }

    if (!expect(EQUALITY)) {
        return unexpected(EQUALITY, "Operand statement after identifier");
    }

    // structure is <TYPELIKE>(|<TYPELIKE>)* ((`||` or `&&`) <TYPELIKE>(|<TYPELIKE>)*)?;
    OperatorInfo* operator_info= OperatorInfo_arr_ptr(&operators, op_i);
    OperandInfo operand_info= (OperandInfo) {
        .op_type= operator_info->op_type,
        .operator= operator_info,
        .explicit_out_type= 0
    };

    if (operand_info.op_type == OIGT_BINARY)
        operand_info.matrix= create_type_matrix();
    else
        operand_info.typemap= 0;

    do {
        uint64_t left= 0;
        parse_type_pipe_statement(&left);

        bool bidirectional= expect(OR);
        if (bidirectional || expect(AND)) {
            uint64_t right= 0;
            errcode r_ret= parse_type_pipe_statement(&right);

            if (r_ret.code != SUCCESS)
                return r_ret;

            type_maps_to_matrix(operand_info.matrix, left, right, bidirectional);
        } else {
            // this is a single valued, so we mark all as compatible with self
            //  e.g. 0 0 1 1 1 1 0 0 0 -> INTEGER x INTEGER, NATURAL x NATURAL
            uint idx=0;
            while (left) {
                while ((left & 1) != 1) {
                    left >>= 1;
                    idx++;
                }

                add_to_op_info(&operand_info, operator_info, idx, idx);

                left >>= 1;
                idx++;
            }
        }

        if (expect(ARROW)) {
            operand_info.explicit_out_type= 1;
            operand_info.unwrapped_output= 0;

            TPPToken* out;
            if (out= expect(IDENTIFIER), out) {
                uint pos= TypeInfo_arr_search_i(&types, out->data.str);
                if (pos == (uint)-1) {
                    return error("Output type must be a defined type. Found `%s`", out->data.str);
                }
                operand_info.output_index= pos;
            } else if (out= expect_keyword(UNWRAP), out) {
                operand_info.unwrapped_output= 1;
            } else {
                return unexpected(IDENTIFIER, "explicit return type of operand statement");
            }
        }
    } while (expect(DELIMITER));

    OperandInfo_arr_add(&operands, operand_info);
    print_operand_info(&operand_info);

    if (!expect(EOS))
        return unexpected(EOS, "End of operand statement");

    return SUCC;
}

errcode parse_identifier_statement(const TPPToken* identifier_token) {
    errcode err;
    switch (section_state) {
        case SECTION_TYPEFIX:
            err= parse_typefix_identifier_statement();
            break;
        case SECTION_TYPES:
            err= parse_types_identifier_statement();
            break;
        case SECTION_OPERATORS:
            err= parse_operators_identifier_statement();
            break;
        case SECTION_ALIASES:
            err= parse_aliases_identifier_statement();
            break;
        case SECTION_COERCIONS:
            err= parse_coercions_identifier_statement();
            break;
        case SECTION_OPERANDS:
            err= parse_operands_identifier_statement();
            break;
        case SECTION_NONE:
            err= error("Expected state header e.g. ALIASES, TYPEFIX, etc before identifier statement. Got identifier `%s`", identifier_token->data.str);
            break;
        default:
            assert(false);
    }

    if (err.code != SUCCESS) {
        if (err.fatal) return err;
        // if we did not successfully parse a line
        //  then just go to the end of the line to skip bad tokens
        TPPToken* c;
        while (c= current(), c && c->type != EOS) consume();
        consume();
    }

    return err;
}

static void or_type_matrices(TypeMatrix dst, TypeMatrix or) {
    for (size_t i= 0; i < type_matrix_bytes(); ++i) {
        dst[i] |= or[i];
    }
}

static void add_to_type_matrix(TypeMatrix matrix, uint32_t x, uint32_t y) {
    uint32_t width= types.pos + typefixes.pos;
    uint64_t index= y * width + x;
    matrix[index / 8] |= 1 << (index & 7);
}

static inline int get_from_type_matrix(TypeMatrix matrix, uint32_t x, uint32_t y) {
    uint32_t width= types.pos + typefixes.pos;
    uint64_t index= y * width + x;
    return (matrix[index / 8] >> (index & 7)) & 1;
}

static TypeMatrix create_type_matrix() {
    TypeMatrix ret= malloc(type_matrix_bytes());

    memset(ret, 0, type_matrix_bytes());

    return ret;
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
           "  Name   : `%s` (`%s`)\n"
           "  Symbol : `%s`\n"
           "  Prefix?: %s\n\n",
           info->name,
           info->alt_name,
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
        for (uint i = 0; i < info->sizes.pos; ++i) {
            const uint size= uint_arr_get(&info->sizes, i);

            printf("    - %u\n", size);
        }
    } else {
        if (info->has_multiple_names) {
            printf("  Names  :\n");
            for (uint i= 0; i < info->names.pos; ++i) {
                const char* name= vector_get_unsafe(&info->names, i);

                printf("    - `%s`\n", name);
            }
        } else {
            printf("  Name   : `%s`\n", info->name);
        }
    }

    printf("  Virtual?: %s\n"
           "  Requirements: %s\n",
           info->is_virtual ? "True" : "False",
           info->requirements.pos == 0 ? "None" : ""
    );

    for (uint i = 0; i < info->requirements.pos; ++i) {
        const uint req_pos= uint_arr_get(&info->requirements, i);
        const TypeFixInfo* req= TypeFixInfo_arr_ptr(&typefixes, req_pos);

        printf("    - `%s`\n", req->name);
    }

    newline();
}

// [[todo]] conv to []
const char* assoc_str(const unsigned char assoc) {
    switch (assoc) {
        case OIA_LEFT : return "LEFT";
        case OIA_RIGHT: return "RIGHT";
        default: assert(false);
    }
}

const char* op_type_str(const unsigned char op_type) {
    switch (op_type) {
        case OIGT_BINARY: return "BINARY";
        case OIGT_POSTFIX: return "POSTFIX";
        case OIGT_PREFIX: return "PREFIX";
        case OIGT_TRINARY: return "TRINARY";
        case OIGT_UNARY: return "UNARY";
        default: assert(false);
    }
}

void print_operator_info(const OperatorInfo* info) {
    printf("Operator:\n"
           "  Name      : `%s`\n"
           "  Symbol    : `%s`%s\n"
           "  Precedence: %u\n"
           "  Assoc     : %s\n"
           "  Op Type   : %s\n\n",
           info->name,
           info->symbol,
           info->symbol_is_ident ? " (IDENT)" : "",
           info->precedence,
           assoc_str(info->assoc),
           op_type_str(info->op_type)
    );
}

_Static_assert(sizeof(uint64_t) * 8 == 64, "Expected uint64_t to be available");
void print_typemap_selected(const char* prefix, const uint64_t map) {
    assert(types.pos + typefixes.pos <= 64);
    // - - - - - 0 1 0 0 1 1 1
    // ^-------^ ^-^ ^-------^
    //  EXCESS  TYFIX  TYPES
    for (uint i= 0; i < typefixes.pos; i++) {
        if ((map >> i) & 1) {
            const TypeFixInfo* typefix= TypeFixInfo_arr_ptr(&typefixes, i);
            printf("%s%s (%s: %s)\n",
                prefix,
                typefix->name,
                typefix->symbol,
                typefix->prefix ? "PREFIX" : "POSTFIX"
            );
        }
    }

    const uint type_start= typefixes.pos;
    for (uint i = 0; i < types.pos; ++i) {
        const uint map_i= type_start + i;
        if ((map >> map_i) & 1) {
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

    for (uint i= 0; i < typefixes.pos; i++) {
        const TypeFixInfo* typefix= TypeFixInfo_arr_ptr(&typefixes, i);
        const bool active= (map >> i) & 1;
        printf("%s%s%-10s"C_RST": %s (%s: %s)\n",
            prefix,
            active ? C_GRN : C_RST,
            typefix->name,
            active ? "X" : "-",
            typefix->symbol,
            typefix->prefix ? "PREFIX" : "POSTFIX"
        );
    }
    const uint types_start= typefixes.pos;
    for (uint i= 0; i < types.pos; ++i) {
        const uint map_i= types_start + i;
        const bool active= (map >> map_i) & 1;

        const TypeInfo* type= TypeInfo_arr_ptr(&types, i);
        printf("%s%s%-10s"C_RST": %s (%s)\n",
            prefix,
            active ? C_GRN : C_RST,
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

const char* get_type_like_name(size_t i) {
    if (i >= typefixes.pos + types.pos) {
        return "INVALID TYPELIKE";
    }
    if (i >= typefixes.pos) {
        return TypeInfo_arr_get(&types, i - typefixes.pos).name;
    }

    return TypeFixInfo_arr_get(&typefixes, i).name;
}

void print_type_matrix_header(const char* prefix, uint typelike_c) {
    putz(prefix);
    putz("x->y  ");
    for (uint i= 0; i < typelike_c; ++i) {
        printf("%02u ", i);
    }
    newline();

    putz(prefix);
    putz("     |");
    for (uint i= 0; i < typelike_c; ++i) {
        putz("---");
    }
    newline();
}

void print_type_matrix_diff(TypeMatrix old, TypeMatrix new) {
    uint typelike_c= type_map_size();

    print_type_matrix_header("", typelike_c);

    for (uint i= 0; i < typelike_c; ++i) {
        printf("  %02u |", i);
        for (uint j = 0; j < typelike_c; ++j) {
            bool o_m= get_from_type_matrix(old, j, i);
            bool n_m= get_from_type_matrix(new, j, i);

            printf("%s%s"C_RST, n_m ? C_GRN : C_RST, (n_m || o_m) ? "-X-" : "   ");
            fflush(stdout);
        }
        printf("%s", get_type_like_name(i));
        newline();
    }
    newline();
}

void print_type_matrix(const char* prefix, const TypeMatrix matrix) {
    assert(types.arr);

    uint typelike_c= type_map_size();

    /*      0  1  2 ... 7  8  9 (typelike_c)
     *    |-----------------------------
     *  0 |
     *  1 |
     *  2 |
     *  ...
     *  7 |
     *  8 |
     *  9 |
     */
    print_type_matrix_header(prefix, typelike_c);

    for (uint i= 0; i < typelike_c; ++i) {
        printf("%s  %02u |", prefix, i);
        for (uint j = 0; j < typelike_c; ++j) {
            bool marked= get_from_type_matrix(matrix, j, i);

            printf("%s", marked ? "-X-" : "   ");
            fflush(stdout);
        }
        printf(" %s", get_type_like_name(i));
        newline();
    }
}

void print_coercion_info(const CoercionInfo* info) {
    printf("Coercions: \n");
    print_type_matrix("", info->matrix);

    newline();
}

void print_operand_info(const OperandInfo* info) {
    printf("Operand info: \n"
        "  Operator: `%s` (%p)\n"
        "  Explicit output?: ",
        info->operator->name,
        info->operator
    );

    if (info->explicit_out_type) {
        if (info->unwrapped_output) {
            putz("YES (Unwrapped typefix)");
        } else {
            TypeInfo* type= TypeInfo_arr_ptr(&types, info->output_index);
            printf("YES (%s)", type->name);
        }
    } else {
        putz("NO (left type)");
    }

    putz("\n  Operand info:\n");
    if (info->op_type == OIGT_BINARY) {
        print_type_matrix("    ", info->matrix);
    } else if (info->op_type == OIGT_POSTFIX || info->op_type == OIGT_PREFIX) {
        print_typemap_all("    ", info->typemap);
    } else {
        assert(false);
    }

    newline();
}

static TPPToken* expect(const TPPType type) {
    if (current()->type == type) {
        return consume();
    }

    return NULL;
}

static TPPToken* expect_keyword(const enum KEYWORDS keyword) {
    const TPPToken* const c= current();
    if (c->type == KEYWORD && c->data.keyword == keyword) {
        return consume();
    }

    return NULL;
}

static bool is_valid_index(uint index) {
    return index < t_tokens.pos;
}

static TPPToken* consume() {
    if (!is_valid_index(t_idx)) return NULL;

    return TPPToken_arr_ptr(&t_tokens, t_idx++);
}

static TPPToken* peek() {
    if (!is_valid_index(t_idx + 1)) return NULL;

    return TPPToken_arr_ptr(&t_tokens, t_idx + 1);
}

static TPPToken* current() {
    if (!is_valid_index(t_idx)) return NULL;

    return TPPToken_arr_ptr(&t_tokens, t_idx);
}

static errcode unexpected(const TPPType expected, const char* context) {
    error(
        "Expected `%s` in %s. Found `",
        get_tpptoken_type_string(expected),
        context
    );
    print_tpptoken(current());
    putchar('`');
    putchar('\n');

    return ERROR(FAIL);
}
