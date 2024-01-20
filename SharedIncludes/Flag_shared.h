//
// Created by jamescoward on 05/11/2023.
//

#ifndef ATOMIC_FLAG_SHARED_H
#define ATOMIC_FLAG_SHARED_H

#include "Helper_String.h"
#include "Helper_Math.h"

typedef long long int llint;

#define MAX_FLAG_SYMBOLS 8

llint flag_to_int(const char* flag);
llint flag_split_to_int(const char* flag_a, const char* flag_b);

#endif //ATOMIC_FLAG_SHARED_H
