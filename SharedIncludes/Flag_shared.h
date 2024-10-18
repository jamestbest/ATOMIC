//
// Created by jamescoward on 05/11/2023.
//

#ifndef ATOMIC_FLAG_SHARED_H
#define ATOMIC_FLAG_SHARED_H

#include "../Commons.h"
#include "Helper_String.h"
#include "../Flags.h"

typedef long long int llint;

#define MAX_FLAG_SYMBOLS 8

bool flag_get_value(const size_t index);
FlagInfo flag_get_info(const size_t index);
const char* flag_get_str(const size_t index);

size_t flag_find(const char* name);
size_t option_find(const char* name);
void flag_set(const size_t index, const bool value);

void print_flags(void);

#endif //ATOMIC_FLAG_SHARED_H
