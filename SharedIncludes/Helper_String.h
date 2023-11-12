//
// Created by jamescoward on 11/11/2023.
//

#ifndef ATOMIC_HELPER_STRING_H
#define ATOMIC_HELPER_STRING_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int uint;

#define SPACE_C ' '

int find_last(char* string, char pattern);
int starts_with_ips(const char* string, const char* pattern);
int starts_with(const char* string, const char* pattern);
bool str_eq(const char* stra, const char* strb);
uint len(const char* string);
uint len_with(const char* string, uint offset);
uint len_from(const char* string, uint offset);
uint len_from_to(const char* string, uint offset, char to);
bool str_contains(const char* str, uint from, uint to, char c);

char* str_cpy(const char* string);
char* str_cpy_replace(const char* string, char find, char replace);

#endif //ATOMIC_HELPER_STRING_H
