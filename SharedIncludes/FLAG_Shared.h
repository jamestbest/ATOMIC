//
// Created by jamescoward on 05/11/2023.
//

#ifndef ATOMIC_FLAG_SHARED_H
#define ATOMIC_FLAG_SHARED_H

typedef unsigned int uint;
#include "helper.h"

long long int flag_to_int(char* flag) {
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

#endif //ATOMIC_FLAG_SHARED_H
