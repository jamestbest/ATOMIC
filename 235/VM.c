//
// Created by jamescoward on 10/09/2025.
//

#include "VM.h"

#include "Kall.h"

#include <assert.h>

bool running= true;
size_t ip= 0;

// 1 MB of stack space
#define STACK_SIZE (1024 * 1024 * 1)

uint64_t registers[MAX_REG + 1]= {0};
uint8_t stack[STACK_SIZE]= {0};

uint64_t ret_reg= 0;

uint64_t* sp= 0;
uint64_t* bp= 0;

typedef struct Frame {
    size_t ret;

    uint64_t scratch;
} Frame;

uint64_t value_to_u64(Value v){
    switch (v.type) {
        case VT_REG:
            if (v.data.reg == RET_REG) return ret_reg;
            if (v.data.reg >= ARG_REG_S && v.data.reg <= ARG_REG_E) {
                return bp[v.data.reg - ARG_REG_S + 1];
            }
            if (v.data.reg <= MAX_REG) return registers[v.data.reg];

            assert(false);
        case VT_IMM_I:
            return v.data.i64;
        case VT_IMM_U:
            return v.data.u64;
        case VT_IMM_F:
            return v.data.f80;
    }

    assert(false);
}

uint64_t size_down(uint8_t size, uint64_t value) {
    switch (size) {
        case 1:
            value= (uint8_t)value;
            break;
        case 2:
            value= (uint16_t)value;
            break;
        case 4:
            value= (uint32_t)value;
            break;
        case 8:
            value= (uint64_t)value;
            break;
    }
    return value;
}

void assign(uint8_t size, Value r, uint8_t res_reg) {
    uint64_t rv= value_to_u64(r);
    uint8_t dest= res_reg;

    registers[dest]= size_down(size, rv);
}

void add(uint8_t size, Value l, Value r, uint8_t res_reg) {
    uint64_t lv= value_to_u64(l);
    uint64_t rv= value_to_u64(r);

    uint64_t res= lv + rv;

    res= size_down(size, res);

    registers[res_reg]= res;
}

void run_arith_bi(ByteStmt* stmt) {
    Value l= Value_arr_get(&stmt->args, 0);
    Value r= Value_arr_get(&stmt->args, 1);

    switch (stmt->sub_idx) {
        case OP_PLUS:
            add(stmt->size, l, r, stmt->dest_reg);
            break;
        case OP_ASSIGN:
            assign(stmt->size, r, l.data.reg);
            break;
        default:
            assert(false);
    }
}

void create_frame(const size_t ret, const ValueArray* args) {
    *sp= (uint64_t)bp;
    sp++;

    bp= sp;
    *sp= ret;
    sp++;

    for (size_t i= 0; i < args->pos; ++i) {
        Value v= Value_arr_get(args, i);

        *sp= value_to_u64(v);
        sp++;
    }

}

int load(FILE* byte_file) {
    return -1;
}

int load_and_run(FILE* byte_file) {
    return -1;
}

int run(ByteStmtArray stmts, StringEntryArray functions) {
    sp= (uint64_t*)&stack;

    while (running && ip < stmts.pos) {
        ByteStmt* stmt= ByteStmt_arr_ptr(&stmts, ip);

        switch (stmt->code) {
            case B_EXIT:
                running= false;
                printf("Program finished with exit code %lld\n", value_to_u64(stmt->arg));
                break;
            case B_RET: {
                ret_reg= value_to_u64(stmt->arg);
                sp= bp;
                ip= *sp;
                sp--;
                bp= (uint64_t*)*sp;
                goto ip_overwritten;
            }
            case B_CALL: {
                create_frame(ip + 1, &stmt->args);
                size_t label= StringEntry_arr_search_ie(&functions, stmt->func_name)->value;
                ip= label + 1;
                goto ip_overwritten;
            }
            case B_LABEL:
                break;
            case B_STORE: {
                Value l= stmt->arg;
                uint8_t dest= stmt->dest_reg;

                registers[dest]= value_to_u64(l);
                break;
            }
            case B_B: {
                ip= stmt->label_id + 1;
                goto ip_overwritten;
            }
            case B_BEQ: {
                Value l= Value_arr_get(&stmt->args, 0);
                Value r= Value_arr_get(&stmt->args, 1);

                uint64_t lv= value_to_u64(l);
                uint64_t rv= value_to_u64(r);

                if (lv == rv) {
                    size_t label= stmt->label_id;
                    ip= label + 1;
                    goto ip_overwritten;
                }
                break;
            }
            case B_ARITH_BI:
                run_arith_bi(stmt);
                break;
            case B_KALL: {
                kall(stmt->sub_idx, stmt->args);
                break;
            }
        }

        ip++;
    ip_overwritten:;
    }

    return SUCCESS;
}
