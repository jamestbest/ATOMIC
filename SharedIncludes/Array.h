//
// Created by jamescoward on 07/06/2024.
//

#ifndef VEC_H
#define VEC_H

#include "../Commons.h"
#include <malloc.h>

#define MIN_VEC_SIZE 10

#define ARRAY_PROTO(type, typename)                                             \
    void typename##_arr_add(Array* arr, const type element);                    \
    type typename##_arr_get(const Array* arr, const uint index);                \
    type typename##_arr_pop(Array* arr);                                        \
    type typename##_arr_peek(const Array* arr);                                 \
    Array typename##_arr_create();

#define ARRAY_ADD(type, typename)                                               \
    void typename##_arr_add(Array* arr, const type element) {                   \
         arr_add(arr, &element);                                                \
    }                                                                           \
                                                                                \
    type typename##_arr_get(const Array* arr, const uint index) {               \
         return *(type*)arr_get(arr, index);                                    \
    }                                                                           \
                                                                                \
    type typename##_arr_pop(Array* arr) {                                       \
        return *(type*)arr_pop(arr);                                            \
    }                                                                           \
                                                                                \
    type typename##_arr_peek(const Array* arr) {                                \
        return *(type*)arr_peek(arr);                                           \
    }                                                                           \
                                                                                \
    Array typename##_arr_create() {                                             \
        return arr_create(sizeof (type));                                       \
    }

typedef struct {
    uint element_size;
    char* arr;
    uint capacity;
    uint pos;
} Array;

Array arr_construct(const uint element_size, const uint min_element_count);
Array arr_create(const uint element_size);
bool arr_is_at_capacity(const Array* vec);
void arr_resize(Array* vec);
void arr_add(Array* vec, const void* element);
bool arr_remove(Array* arr, const uint index);
void* arr_pop(Array* arr);
void* arr_peek(const Array* arr);
void* arr_get(const Array* arr, const uint index);
void arr_destroy(Array* vec);

#endif //VEC_H
