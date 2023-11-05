//
// Created by jamescoward on 05/11/2023.
//

#ifndef ATOMIC_FLAG_SHARED_H
#define ATOMIC_FLAG_SHARED_H

typedef unsigned int uint;
#include "helper.h"

long long int ATOM__FLAG_TO_INT(char* flag) {
    uint flag_len = len(flag);
    uint itters = umin(flag_len, 8);

    long long int out = 0;
    for (int i = 0; i < itters; i++) {
        out |= (((long long int)flag[i]) << (i * 8));
    }

    return out;
}

#endif //ATOMIC_FLAG_SHARED_H
