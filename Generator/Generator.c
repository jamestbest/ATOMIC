//
// Created by jamescoward on 09/09/2025.
//

#include "GeneratorInternal.h"

#include <235/Kall.h>

uint8_t next_reg= 0;
uint8_t freed_reg= NO_FREED;
uint64_t reg_map= 0;

StringEntryArray function_labels;
StringEntryArray variable_registers;

ByteStmtArray stmts;

void add_function(const char* func, size_t value) {
    StringEntry_arr_add(&function_labels, (StringEntry){.string= func, .value= value});
}

void add_variable(const char* variable, size_t reg) {
    StringEntry_arr_add(&variable_registers, (StringEntry){.string= variable, .value= reg});
}

size_t find_function(const char* func) {
    StringEntry* res= StringEntry_arr_search_ie(&function_labels, func);
    if (res) return res->value;

    return -1;
}

size_t find_variable(const char* variable) {
    return StringEntry_arr_search_ie(&variable_registers, variable)->value;
}

void release_reg(uint8_t reg) {
    reg_map ^= (1 << reg);
    if (freed_reg > reg) freed_reg= reg;
}

void set_next_free() {
    if (reg_map == 0xFF) next_reg= NO_FREED;

    for (uint8_t i= 0; i < MAX_REG; ++i) {
        if (!((reg_map >> i) & 1)) {
            next_reg= i;
            return;
        }
    }
    assert(false);
}

uint8_t get_free_reg() {
    if (freed_reg != NO_FREED) {
        reg_map |= 1 << freed_reg;
        uint ret= freed_reg;
        set_next_free();
        return ret;
    }

    if (next_reg <= MAX_REG) {
        reg_map |= 1 << next_reg;
        return next_reg++;
    }

    if (reg_map == 0xFF) {
        assert(false);
    }

    assert(false);
}

void add_stmt(ByteStmt stmt) {
    ByteStmt_arr_add(&stmts, stmt);
}

void generate_mem_op(BYTE_CODE op, uint size, Value value, uint dest_reg) {
    ByteStmt_arr_add(&stmts,
         (ByteStmt) {
            .code= op,
            .size= size,
            .arg= value,
            .dest_reg= dest_reg
         }
    );
}

#define LABEL_UNSET ((uint)-1)
// the return value is the index in the statements array that the label exists at
uint generate_label() {
    uint label_id= stmts.pos;
    ByteStmt_arr_add(&stmts,
    (ByteStmt) {
        .code= B_LABEL,
        .label_id= label_id
    });
    return label_id;
}

size_t generate_branch(size_t label) {
    ByteStmt_arr_add(&stmts,
        (ByteStmt){
            .code= B_B,
            .label_id= label
        }
    );

    return stmts.pos - 1;
}

size_t generate_cmp(BYTE_CODE op, Value left, Value right, size_t label) {
    size_t res= stmts.pos;
    ByteStmt* stmt= ByteStmt_arr_add_i(&stmts);

    *stmt= (ByteStmt) {
        .code= op,
        .args= Value_arr_construct(3)
    };

    Value_arr_add(&stmt->args, left);
    Value_arr_add(&stmt->args, right);

    stmt->label_id= label;

    return res;
}

void generate_register_op(BYTE_CODE op, size_t size, Value arg, uint8_t reg) {
    ByteStmt_arr_add(
        &stmts,
        (ByteStmt) {
            .code= op,
            .size= size,
            .arg= arg,
            .dest_reg= reg
    });
}

void generate_bin_arith_op(ATOM_CT__LEX_OPERATORS_ENUM sub_op, size_t size, Value l, Value r, uint8_t reg) {
    ByteStmt stmt= {
        .code= B_ARITH_BI,
        .sub_idx= sub_op,
        .size= size,
        .args= Value_arr_construct(2),
        .dest_reg= reg
    };

    Value_arr_add(&stmt.args, l);
    Value_arr_add(&stmt.args, r);

    ByteStmt_arr_add(&stmts, stmt);
}

void generate_ret(size_t size, Value v) {
    ByteStmt_arr_add(
        &stmts,
        (ByteStmt) {
            .code= B_RET,
            .size= size,
            .arg= v
       }
    );
}

void generate_fcall(const char* function, ValueArray args) {
    ByteStmt_arr_add(
        &stmts,
        (ByteStmt) {
            .code= B_CALL,
            .func_name= function,
            .args= args,
            .dest_reg= RET_REG
       }
    );
}

Value lit_i(int64_t v) {
    return (Value){
        .type= VT_IMM_I,
        .data.i64= v
    };
}

Value lit_f(long double v) {
    return (Value){
        .type= VT_IMM_F,
        .data.f80= v
    };
}

Value reg(uint8_t v) {
    return (Value){
        .type= VT_REG,
        .data.reg= v
    };
}

Value arg(uint8_t arg_no) {
    assert(arg_no >= 0 && arg_no + ARG_REG_S <= ARG_REG_E);

    return (Value){
        .type= VT_REG,
        .data.reg= arg_no + ARG_REG_S
    };
}

Value label(size_t v) {
    return (Value){
        .type= VT_LABEL,
        .data.label= v
    };
}

void free_value(Value* v) {
    if (v->type == VT_REG) {
        release_reg(v->data.reg);
    }
}

void generate_exit(Value code) {
    ByteStmt_arr_add(
        &stmts,
        (ByteStmt) {
            .code= B_EXIT,
            .arg= code
        }
    );
}

uint generate_expr(Node* expr);

Value create_value(Node* expr) {
    switch (expr->type) {
        case EX_LIT: {
            switch (expr->token->type) {
                case LIT_INT:
                    return lit_i(expr->token->data.integer);
                case LIT_FLOAT:
                    return lit_f(expr->token->data.real);
                case IDENTIFIER:
                    return reg(find_variable(expr->token->data.ptr));
                default:
                    assert(false);
            }
        }
        case EXPR_ASSIGNMENT: {
            Node* l= vector_get_unsafe(&expr->children, 0);
            Node* r= vector_get_unsafe(&expr->children, 1);

            Value lv= create_value(l);
            Value rv= create_value(r);

            // todo this will cause issues if any are just a variable
            //            free_value(&lv);
            //            free_value(&rv);

            generate_mem_op(B_STORE, 8, rv, lv.data.reg);
//            generate_assign_op(expr->token->data.enum_pos, 8, lv, rv);

            if (lv.type != VT_REG)
                assert(false);

            return lv;
        }
        case EXPR_BINARY: {
            Node* l= vector_get_unsafe(&expr->children, 0);
            Node* r= vector_get_unsafe(&expr->children, 1);

            Value lv= create_value(l);
            Value rv= create_value(r);

            uint8_t res_reg= get_free_reg();

            // todo this will cause issues if any are just a variable
//            free_value(&lv);
//            free_value(&rv);

            generate_bin_arith_op(expr->token->data.enum_pos, 8, lv, rv, res_reg);

            return reg(res_reg);
        }
        case SUB_CALL: {
            size_t arg_reg= ARG_REG_S;

            Node* ident= vector_get_unsafe(&expr->children, 0);
            Node* args= vector_get_unsafe(&expr->children, 1);

            if (args->children.pos == 0) {
                generate_fcall(ident->token->data.ptr, ValueARRAY_EMPTY);
            } else {
                Node* f_link= ident->link;
                Node* f_params= vector_get_unsafe(&f_link->children, 1);

                ValueArray t_args= Value_arr_construct(args->children.pos);
                for (size_t i= 0; i < args->children.pos; ++i) {
                    Node* a_expr= vector_get_unsafe(&args->children, i);
                    Node* f_param= vector_get_unsafe(&f_params->children, i);

                    add_variable(f_param->token->data.ptr, arg_reg);

                    Value a= create_value(a_expr);
                    Value_arr_add(&t_args, a);

                    arg_reg++;
                }
                generate_fcall(ident->token->data.ptr, t_args);
            }
            return reg(RET_REG);
        }
    }

    assert(false);
}

// the return value is in which register the result is stored
// use generate expr over create value when you need the result in a register
//  create value could be an immediate
uint generate_expr(Node* expr) {
    switch (expr->type) {
        case EX_LIT: {
            Value v= create_value(expr);
            uint8_t dest_reg= get_free_reg();
            generate_register_op(B_STORE, 8, v, dest_reg);

            return dest_reg;
        }
        case EXPR_ASSIGNMENT:
        case EXPR_BINARY: {
            return create_value(expr).data.reg;
        }
        case ST_EXPR: {
            Node* c_expr= vector_get_unsafe(&expr->children, 0);
            return generate_expr(c_expr);
        }
        case SUB_CALL: {
            return create_value(expr).data.reg;
        }
        default:
            assert(false);
    }

    assert(false);
}

int generate_kall(const KALL code) {
    ValueArray v= Value_arr_construct(1);
    const uint8_t dst_reg= get_free_reg();

    Value_arr_add(&v, (Value){.type= VT_IMM_U, .data.u64= 12});
    ByteStmt_arr_add(
        &stmts,
        (ByteStmt) {
            .code= B_KALL,
            .sub_idx= code,
            .args= v,
            .dest_reg= dst_reg
        }
    );
    return SUCCESS;
}

int generate_stmts(Node* stmt) {
    if (stmt->gtype == DECLARATION) return SUCCESS;

    if (stmt->type == ST_VAR_DECL) {
        Node* ident= vector_get_unsafe(&stmt->children, 0);
        Node* type= vector_get_unsafe(&stmt->children, 1);

        if (stmt->children.pos > 2) {
            Node* expr= vector_get_unsafe(&stmt->children, 2);
            uint res_reg= generate_expr(expr);

            add_variable(ident->token->data.ptr, res_reg);
        }

        return SUCCESS;
    }

    if (stmt->type == ST_EXPR) {
        generate_expr(stmt);
        return SUCCESS;
    }

    if (stmt->type == ST_FUNC || stmt->type == ST_PROC) {
        const Node* ident= vector_get_unsafe(&stmt->children, 0);
        const size_t f_label= generate_label();

        // generate_kall(K_PRINTX); // todo remove

        add_function(ident->token->data.ptr, f_label);

        const uint64_t rm_save= reg_map;
        const uint8_t nr_save= next_reg;
        const uint8_t fr_save= freed_reg;

        for (uint i= 0; i < stmt->children.pos; ++i) {
            Node* child= vector_get_unsafe(&stmt->children, i);
            generate_stmts(child);
        }

        reg_map= rm_save;
        next_reg= nr_save;
        freed_reg= fr_save;

        return SUCCESS;
    }

    if (stmt->type == ST_TIMES) {
        Node* cmp= vector_get_unsafe(&stmt->children, 0);
        Value max= create_value(cmp);

        uint max_reg= get_free_reg();
        generate_mem_op(B_STORE, 8, max, max_reg);

        uint count_reg= get_free_reg();
        generate_mem_op(B_STORE, 8, lit_i(0), count_reg);
        uint start_label= generate_label();
        size_t cmp_idx= generate_cmp(
            B_BEQ,
            reg(count_reg),
            reg(max_reg),
            LABEL_UNSET
        );
        Node* c_s= vector_get_unsafe(&stmt->children, 1);
        generate_stmts(c_s);
        generate_bin_arith_op(OP_PLUS, 8, reg(count_reg), lit_i(1), count_reg);
        generate_branch(start_label);
        uint end_label= generate_label();
        ByteStmt_arr_ptr(&stmts, cmp_idx)->label_id= end_label;

        return SUCCESS;
    }

    if (stmt->type == ST_RET) {
        Node* expr= vector_get_unsafe(&stmt->children, 0);
        generate_ret(8, create_value(expr));
        return SUCCESS;
    }

    for (uint i= 0; i < stmt->children.pos; ++i) {
        Node* child= vector_get_unsafe(&stmt->children, i);
        generate_stmts(child);
    }

    return SUCCESS;
}

void write_reg(FILE* out, uint8_t reg) {
    if (reg >= 0 && reg <= MAX_REG) {
        fprintf(out, "R%u", reg);
        return;
    }

    if (reg >= ARG_REG_S && reg <= ARG_REG_E) {
        fprintf(out, "A%u", reg - ARG_REG_S);
        return;
    }

    if (reg == RET_REG) {
        fprintf(out, "R00");
        return;
    }

    if (reg == (uint8_t)NO_REG) {
        return;
    }

    assert(false);
}

void write_value(FILE* out, Value* v, const char* post) {
    switch (v->type) {
        case VT_IMM_I:
            fprintf(out, "%lld", v->data.i64);
            break;
        case VT_IMM_F:
            fprintf(out, "%Lf", v->data.f80);
            break;
        case VT_IMM_U:
            fprintf(out, "%llu", v->data.u64);
            break;
        case VT_LABEL:
            fprintf(out, "L%zu", v->data.label);
            break;
        case VT_REG:
            write_reg(out, v->data.reg);
            break;
        default:
            assert(false);
    }
    fputs(post, out);
}

const char* op_enum_to_op_string(ATOM_CT__LEX_OPERATORS_ENUM op) {
    switch (op) {
        case OP_PLUS:
            return "ADD";
        case OP_MINUS:
            return "SUB";
        case OP_ASSIGN:
            return "STORE";
        default:
            assert(false);
    }
}

void write_args(FILE* out, ValueArray* args, const char* post) {
    for (size_t i= 0; i < args->pos; ++i) {
        write_value(out, Value_arr_ptr(args, i), i != args->pos - 1 ? ", " : "");
    }
    fputs(post, out);
}

void write_byte_code(FILE* out) {
    fprintf(out, "--BYTE-CODE--\n");
    for (size_t i= 0; i < stmts.pos; ++i) {
        ByteStmt* stmt= ByteStmt_arr_ptr(&stmts, i);

        switch (stmt->code) {
            case B_B:
                fprintf(out, "B L%zu", stmt->label_id);
                break;
            case B_RET:
                fprintf(out, "RET%u ", stmt->size);
                write_value(out, &stmt->arg, "");
                break;
            case B_ARITH_BI:
                fprintf(out, "%s%u ", op_enum_to_op_string(stmt->sub_idx), stmt->size);
                write_args(out, &stmt->args, ", ");
                write_reg(out, stmt->dest_reg);
                break;
            case B_LABEL:
                fprintf(out, ":L%zu:", stmt->label_id);
                break;
            case B_BEQ:
                fprintf(out, "BEQ ");
                write_args(out, &stmt->args, ", ");
                fprintf(out, "L%zu", stmt->label_id);
                break;
            case B_STORE:
                fprintf(out, "STORE%u ", stmt->size);
                write_value(out, &stmt->arg, ", ");
                write_reg(out, stmt->dest_reg);
                break;
            case B_CALL:
                fprintf(out, "CALL %s", stmt->func_name);
                if (stmt->args.pos)
                    fprintf(out, ", ");
                write_args(out, &stmt->args, "");
                break;
            case B_EXIT:
                fprintf(out, "EXIT ");
                write_value(out, &stmt->arg, "");
                break;
            case B_KALL:
                fprintf(out, "KALL %s ", KALL_STRINGS[stmt->sub_idx]);
                write_args(out, &stmt->args, ", ");
                write_reg(out, stmt->dest_reg);
                break;
            default:
                fprintf(out, "MISSING WITH VALUE %d", stmt->code);
                break;
        }
        fnewline(out);
    }
    fprintf(out, "--BYTE-CODE-END--\n");
}

#define ATOM_BYTE_CODE_VERSION "1"
void write_byte_code_header(FILE* out) {
    fprintf(
        out,
        "--ATOM-BYTE-CODE--\n"
        "--HEADER--\n"
        "V: "ATOM_BYTE_CODE_VERSION"\n"
        "STRUCTURE:\n"
        "- FUNC-LABEL-LINK\n"
        "- BYTE-CODE\n"
        "--HEADER-END--\n"
    );
}

void write_byte_code_function_links(FILE* out) {
    fprintf(out, "--FUNC-LABEL-LINK--\n");
    StringEntry_arr_sort_i(&function_labels);
    for (size_t i= 0; i < function_labels.pos; ++i) {
        StringEntry* entry= StringEntry_arr_ptr(&function_labels, i);
        fprintf(out, "- %s: %zu\n", entry->string, entry->value);
    }
    fprintf(out, "--FUNC-LABEL-LINK-END--\n");
}

void write_byte_code_footer(FILE* out) {
    fprintf(out, "--ATOM-BYTE-CODE-END--\n\n");
}

GeneratorRet generate_byte_code(Node* root, const char* entry_function, FILE* out) {
    stmts= ByteStmt_arr_create();
    function_labels= StringEntry_arr_create();
    variable_registers= StringEntry_arr_create();

    generate_fcall("main", ValueARRAY_EMPTY);
    generate_exit((Value){.type= VT_REG, .data.reg= RET_REG});
    generate_stmts(root);

    write_byte_code_header(out);
    write_byte_code_function_links(out);
    write_byte_code(out);
    write_byte_code_footer(out);

    return (GeneratorRet){.err= SUCC, .arr=stmts, .function= function_labels};
}