//
// Created by jamescoward on 08/11/2023.
//

#include "Flag_shared.h"

long long int flag_to_int(const char* flag) {
    uint flag_len = len(flag);
    uint itters = umin(flag_len, 8);

    long long int out = 0;
    for (int i = 0; i < itters; i++) {
        //potential issue with another character being mapped? -- MAPS TO CR is this an issue? can you enter a CR without running the prog? probably
        /* 1. set 32 to 0 to set to capital
         * 2. shift left to section, each byte has its own area in the llint
         * 3. OR with out
         *
         * essentially just packing upto 8 characters and then interpreting as an int
         */
        out |= ((long long int)(flag[i] & ~(1 << 5))) << (i << 3);
    }

    return out;
}
