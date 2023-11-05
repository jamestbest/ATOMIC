//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_FLAGS_H
#define ATOMIC_FLAGS_H

#include <stdbool.h>
#include "helper.h"

//flags
//__FLAGS[__FLAG_00] = true;
//`__` is reserved for start so `ATOM__` prefix

//ATOM__FLAG_ is a prefix
//THE NEXT 8 CHARACTERS ARE USED IN THE UNIQUE HASH
//64 bit number - 8 characters in a row

/*
 * Go through each flag defined and create a struct that stores the hash and the index?
 */

int ATOM__FLAG_TO_INT(char* flag) {
    uint flag_len = len(flag);
    uint itters = umin(flag_len, 8);

    int out = 0;
    for (int i = 0; i < itters; i++) {
        out |= (flag[i] << (itters * 8));
    }

    return out;
}

//%%FLAG ENUM%%
