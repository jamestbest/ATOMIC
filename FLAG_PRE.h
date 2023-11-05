//
// Created by jamescoward on 04/11/2023.
//

#ifndef ATOMIC_FLAG_PRE_H
#define ATOMIC_FLAG_PRE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"
#include "FLAG_Shared.h"

#define ATOM__FLAG_PRE_END          "//%%FLAG END%%"
#define ATOM__FLAG_PRE_DEF_START    "//%%FLAG DEFINE%%"
#define ATOM__FLAG_PRE_SWT_START    "//%%FLAG SWITCH%%"
#define ATOM__FLAG_PRE_ENUM_START   "//%%FLAG ENUM%%"
#define ATOM__FLAG_PRE_FLAG_REG     "ATOM__FLAG_"

typedef enum State {
    SEARCHING,
    FOUND_ENUM,
    FOUND_HASH,
    FOUND_SWITCH,
} State;

typedef struct Enums {
    char** enumNames;
    uint count;
} Enums;

#define BUFF_SIZE 100
#define ENUM_BUFF_MIN_SIZE 10

Enums readEnums(FILE* fptr, FILE* nfile, char buff[BUFF_SIZE]);
void parseDefs(FILE* nfile, Enums enums);
void parseSwitch(FILE* nfile, Enums enums);
void cleanup(FILE* fptr, FILE* nfile, char* buff);

#endif //ATOMIC_FLAG_PRE_H
