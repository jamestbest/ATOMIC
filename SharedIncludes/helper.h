//
// Created by jamescoward on 04/11/2023.
//
#ifndef ATOMIC_HELPER_H
#define ATOMIC_HELPER_H

#include <stdbool.h>

typedef unsigned int uint;

#define SPACE_C ' '

uint umin(uint a, uint b) {
    return a < b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

uint umax(uint a, uint b) {
    return a > b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

unsigned int len(const char* string) {
    int i = 0;
    while (string[i] != '\0') {i++;};
    return i;
}

bool streq(const char* stra, const char* strb) {
    int i = 0;
    while (stra[i] != '\0' && strb[i] != '\0') {
        if (stra[i] != strb[i]) return false;
        i++;
    }

    return stra[i] == strb[i]; //have both strings ended?
}

//returns -1 for false, otherwise it returns the position in the string that it ended at (len(pattern))
int startswith(const char* string, const char* pattern) {
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
int startswith_ips(const char* string, const char* pattern) {
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

#endif //ATOMIC_HELPER_H
