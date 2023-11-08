//
// Created by james on 07/11/23.
//

#ifndef ATOMIC_VECTOR_H
#define ATOMIC_VECTOR_H

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define MIN_VEC_SIZE 16

#define VEC_ADD(type, typename)                                     \
    VEC_DEF(type, typename)                                         \
    VEC_FN_ADD(type, typename)                                      \
    VEC_FN_GET(type, typename)                                      \
    VEC_FN_CREATE(type, typename)                                   \
    VEC_FN_DESTROY(type, typename)

#define VEC_DEF(type, typename)                                     \
    typedef struct typename##_vec {                                 \
        type* arr;                                                  \
        size_t size;                                                \
        size_t pos;                                                 \
    } typename##_vec;

#define VEC_FN_ADD(type, typename)                                  \
    bool typename##_vec_add(typename##_vec* vector, type toAdd) {   \
        if (vector->pos == vector->size) {                          \
            int new_size = (int) ((double) vector->size * 1.5);     \
            type* out = realloc(vector->arr, new_size);             \
                                                                    \
            if (out == NULL) {                                      \
                return false;                                       \
            }                                                       \
                                                                    \
            vector->size = new_size;                                \
            vector->arr = out;                                      \
        }                                                           \
        vector->arr[vector->pos++] = toAdd;                         \
                                                                    \
        return true;                                                \
    }

#define VEC_FN_GET(type, typename)                                  \
    type typename##_vec_get(typename##_vec* vector, uint index) {   \
        if (index >= vector->size) assert(false);                   \
                                                                    \
        return vector->arr[index];                                  \
    }

#define VEC_FN_CREATE(type, typename)                               \
    typename##_vec typename##_vec_create() {                        \
        type* arr = (type*) malloc(MIN_VEC_SIZE * sizeof(type));    \
                                                                    \
        if (arr == NULL) {                                          \
            return (typename##_vec) {NULL, 0, -1};                  \
        }                                                           \
        return (typename##_vec) {arr, MIN_VEC_SIZE, 0};             \
    }

#define VEC_FN_DESTROY(type, typename)                              \
    void typename##_vec_destroy(typename##_vec* vector) {           \
        free(vector->arr);                                          \
        vector->size = -1;                                          \
        vector->pos = -1;                                           \
    }

#endif //ATOMIC_VECTOR_H
