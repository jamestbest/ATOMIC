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

char lower_unsafe(char a) {
    return (char)(a | 32);
}

char lower(char a) {
    if (is_alph(a)) {
        return lower_unsafe(a);
    }
    return a;
}

//returns -1 for false, otherwise it returns the position in the string that it ended at (len(pattern))
int starts_with(const char* string, const char* pattern) {
    //does the string start with pattern
    int i = 0;

    while (string[i] != '\0' && pattern[i] != '\0') {
        if (string[i] != pattern[i]) return -1;

        i++;
    }

    if (pattern[i] == '\0') return i;

    return -1;
}

//ignore case
int starts_with_ic(const char* string, const char* pattern) {
    int i = 0;

    while (string[i] != '\0' && pattern[i] != '\0') {
        if (lower(string[i]) != lower(pattern[i])) return -1;

        i++;
    }

    if (pattern[i] == '\0') return i;

    return -1;
}

//ignore preceding space
/*
 * RETURNS -1 for string doesn't start with pattern
 * RETURNS position in string where the pattern ends
 */
int starts_with_ips(const char* string, const char* pattern) {
    bool is = true; //ignore space

    uint lp = len(pattern);
    uint ls = len(string);

    if (lp > ls) return -1;

    uint string_p = 0; //string pointer
    for (uint i = 0; i < lp;) {
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
    return (int)string_p;
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

    for (uint i = 0; i < ls; i++) {
        if (string[i] == pattern) out = i;
    }

    return out;
}

char* str_cpy(const char* string) {
    uint length = len(string);

    char* out = malloc((length + 1) * sizeof(char));

    if (out == NULL) return NULL;

    memcpy(out, string, length);

    out[length] = '\0';

    return out;
}

char* str_cpy_replace(const char* string, char find, char replace) {
    uint length = len(string);

    char* out = malloc((length + 1) * sizeof(char));

    if (out == NULL) return NULL;

    for (uint i = 0; i < length; i++) {
        if (string[i] == find) out[i] = replace;
        else out[i] = string[i];
    }

    out[length] = '\0';

    return out;
}

bool is_digit(uint32_t a) {
    return ((uint32_t)(a - ASCII_DIGIT_MIN)) < NUM_DIGITS;
}

bool is_digit_base(uint32_t a, uint base) {
    if (base == 0) return false;
    if (base <= 10) {
        return ((uint32_t)(a - ASCII_DIGIT_MIN)) < base;
    }

    if (is_digit(a)) return true;

    uint alph_count = base - 10;
    return ((uint32_t)(a - ASCII_ALPH_CAP_MIN)) < alph_count ||
            ((uint32_t)(a - ASCII_ALPH_LOW_MIN)) < alph_count;
}

bool is_alph_cap(uint32_t a) {
    return ((uint32_t)(a - ASCII_ALPH_CAP_MIN)) < NUM_ALPH;
}

bool is_alph_low(uint32_t a) {
    return ((uint32_t)(a - ASCII_ALPH_LOW_MIN)) < NUM_ALPH;
}

bool is_alph(uint32_t a) {
    return is_alph_low(a) || is_alph_cap(a);
}

bool is_alph_numeric(uint32_t a) {
    return is_alph(a) || is_digit(a);
}

bool is_whitespace(char a) {
    return a == ' ' || a == '\t';
}

bool is_newline(char a) {
    return a == '\n';
}
