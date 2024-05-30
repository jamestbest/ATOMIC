//
// Created by jamescoward on 08/11/2023.
//

#include "Flag_shared.h"

llint flag_to_int(const char* flag) {
    uint flag_len = len(flag);

    llint out = 0;

    for (uint i = 0; i < flag_len; i++) {
        if (i >= MAX_FLAG_SYMBOLS) {
            break;
        }

        if (flag[i] == '-') continue;
        //potential issue with another character being mapped? -- MAPS TO CR is this an issue? can you enter a CR without running the prog? probably
        /* 1. set 32 to 0 to set to capital
         * 2. shift left to section, each byte has its own area in the llint
         * 3. OR with out
         *
         * essentially just packing upto 8 characters and then interpreting as an int
         */
        out |= ((llint)(flag[i] & ~(1 << 5))) << (i << 3);
    }

    return out;
}

llint flag_split_to_int(const char* flag_a, const char* flag_b) {
    uint l1 = len(flag_a);

    llint a = flag_to_int(flag_a);

    llint out = a | (flag_to_int(flag_b) << (l1 << 3));

    return out;
}
