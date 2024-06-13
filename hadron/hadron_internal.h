//
// Created by jamescoward on 07/06/2024.
//

#ifndef HADRON_INTERNAL_H
#define HADRON_INTERNAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../SharedIncludes/Colours.h"

#ifndef uint
typedef unsigned int uint;
#endif

#define STR_HELPER(x) #x
#define STRINGIFY(x) STR_HELPER(x)

const char SPECIAL_SEQUENCE[] = "ABC 123 !?#";
#define SPECIAL_SEQUENCE_LENGTH 12 // I don't know how to do this with the preprocessor; I can't get sizeof to expand at preprocessor time, because it doesn't, its a comp time

typedef enum {
    INFORM,
    ERROR,
    WARNING
} MessageType;

typedef struct
{
    void** arr;
    uint pos;
    uint capacity;
} Vector;

typedef struct {
    const char* file_name;
    uint line_number;
    const char* func_sig;
} Context;

typedef struct {
    uint64_t allocated;
    uint64_t requested;
} Bytes;

typedef struct {
    Bytes total;
    Bytes total_errors;

    Bytes nonfree;
    Bytes doublefree;
    Bytes corruptSpecial;
    Bytes corruptId;
} ByteCount;

typedef struct {
    uint64_t nonfrees;
    uint64_t doublefrees;
    uint64_t corruptSpecials;
    uint64_t corruptIds;

    uint64_t witherrors;
} Counts;

typedef struct {
    uint line_number;
    const char* func_sig;

    ByteCount bytes;
    Counts counts;
} FuncContext;

typedef struct {
    const char* file_name;
    Vector functions;

    ByteCount bytes;
    Counts counts;
} FileContext;

typedef struct
{
    uint id;

    Context initializationContext;

    uint size_requested;
    uint size_allocated;

    void* address_base; // address from malloc
    void* address_given; // address given to user
    void* address_of_special_sequence;

    uint malloc_count; // includes both malloc & realloc
    uint free_count;

    Context lastFreeContext;
    uint realloc_of_id;
    uint realloced_to_id;
} MallocInfo;

typedef struct {
    MallocInfo* info;
    uint index;
} MallocInfoPosition;

static Vector create_vector();

static uint current_id;

#define MIN_MALLOC_INFO_ARRAY_SIZE 10
static Vector malloc_info_array = (Vector) {
    .arr = NULL,
    .pos = -1,
    .capacity = -1
};

bool HADRON_EXPAND = false;
bool HADRON_RETAIN = false;
bool HADRON_VERBOSE = false;

#endif //HADRON_INTERNAL_H
