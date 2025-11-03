//
// Created by jamescoward on 15/09/2025.
//

#ifndef ATOMIC_KALL_H
#define ATOMIC_KALL_H

#include "Generator/Generator.h"

typedef enum KALL {
    K_PRINT,
    K_PRINTLN,
    K_PRINTX,
    K_MAX
} KALL;

extern const char* KALL_STRINGS[K_MAX];

Value kall(KALL k_code, ValueArray args);

#endif // ATOMIC_KALL_H
