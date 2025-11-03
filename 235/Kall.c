//
// Created by jamescoward on 15/09/2025.
//

#include "Kall.h"

#include <stdio.h>

// todo find correct .h linking
uint64_t value_to_u64(Value v);

const char* KALL_STRINGS[K_MAX]= {
    [K_PRINTX]= "PRINTX"
};

Value print(const char* string) {
    return (Value){.type= VT_IMM_U, .data.u64= printf("%s", string)};
}

Value println(const char* string) {
    return (Value){.type= VT_IMM_U, .data.u64= printf("%s\n", string)};
}

Value print_x(const uint64_t value) {
    return (Value){.type= VT_IMM_U, .data.u64= printf("%0llx", value)};
}

Value kall(KALL code, ValueArray args) {
    switch (code) {
        case K_PRINT: {
            Value string= Value_arr_get(&args, 0);
            return print((const char*)string.data.u64);
        }
        case K_PRINTLN: {
            Value string= Value_arr_get(&args, 0);
            return println((const char*)string.data.u64);
        }
        case K_PRINTX: {
            Value v= Value_arr_get(&args, 0);
            return print_x(value_to_u64(v));
        }
        default:
            assert(false);
    }
}
