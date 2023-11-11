//
// Created by jamescoward on 11/11/2023.
//

#include "Helper_String.h"

uint len(const char* string) {
    int i = 0;
    while (string[i] != '\0') {i++;};
    return i;
}

uint len_with(const char* string, uint offset) {
    uint i = 0;
    while (string[i] != '\0') {i++;};
    return i + offset;
}

uint len_from(const char* string, uint offset) {
    uint i = offset;
    while (string[i] != '\0') {i++;};
    return i - offset;
}

uint len_from_to(const char* string, uint offset, char to) {
    uint i = offset;
    while (string[i] != '\0' && string[i] != to) {i++;};
    return i - offset;
}

bool str_eq(const char* stra, const char* strb) {
    int i = 0;
    while (stra[i] != '\0' && strb[i] != '\0') {
        if (stra[i] != strb[i]) return false;
        i++;
    }

    return stra[i] == strb[i]; //have both strings ended?
}

//returns -1 for false, otherwise it returns the position in the string that it ended at (len(pattern))
int starts_with(const char* string, const char* pattern) {
    //does the string start with pattern

    uint lp = len(pattern);
    uint ls = len(string);

    if (lp > ls) return -1;

    for (int i = 0; i < lp; i++){
        if (string[i] != pattern[i]) return -1;
    }
    return (int) lp;
}

//ignore preceding space
/*
 * RETURNS -1 for string doesn't start with pattern
 * RETURNS position in string where in the string the pattern ends
 */
int starts_with_ips(const char* string, const char* pattern) {
    bool is = true; //ignore space

    uint lp = len(pattern);
    uint ls = len(string);

    if (lp > ls) return -1;

    int string_p = 0; //string pointer
    for (int i = 0; i < lp;) {
        if (string_p > ls) return -1;
        if (is) {
            if (string[string_p] == SPACE_C) {
                string_p++;
            } else {
                is = false;
            }
        } else {
            if (string[string_p] != pattern[i]) return -1;
            else {
                string_p++;
            }
            i++;
        }
    }
    return string_p;
}

bool str_contains(const char* str, uint from, uint to, char c) {
    for (uint i = from; i < to; i++) {
        if (str[i] == '\0') return false;
        if (str[i] == c) return true;
    }
    return false;
}

int find_last(char* string, char pattern) {
    int out = -1;

    uint ls = len(string);

    for (int i = 0; i < ls; i++) {
        if (string[i] == pattern) out = i;
    }

    return out;
}