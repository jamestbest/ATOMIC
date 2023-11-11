//
// Created by james on 07/11/23.
//

#ifndef ATOMIC_VECTOR_H
#define ATOMIC_VECTOR_H

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef unsigned int uint;

#define VEC_ADD(type, typename)                                     \
    VEC_FN_ADD(type, typename)                                      \
    VEC_FN_GET(type, typename)                                      \
    VEC_FN_CREATE(type, typename)                                   \
    VEC_FN_GROW(type, typename)                                     \
    VEC_FN_DESTROY(type, typename)                                  \
    VEC_FN_STEAL(type, typename)                                    \
    VEC_FN_POP(type, typename)


#define VEC_DEF(type, typename)                                     \
    typedef struct typename##_vec {                                 \
        type* arr;                                                  \
        size_t size;                                                \
        size_t pos;                                                 \
    } typename##_vec;

#define VEC_PROTO(type, typename)                                   \
    VEC_DEF(type, typename)                                         \
    VEC_FN_PROTO_ADD(type, typename)                                \
    VEC_FN_PROTO_GET(type, typename)                                \
    VEC_FN_PROTO_CREATE(type, typename)                             \
    VEC_FN_PROTO_GROW(type, typename)                               \
    VEC_FN_PROTO_DESTROY(type, typename)                            \
    VEC_FN_PROTO_STEAL(type, typename)                              \
    VEC_FN_PROTO_POP(type, typename)


#define VEC_FN_PROTO_GROW(type, typename)                           \
    int typename##_vec_grow(typename##_vec* vector, uint n_size);

#define VEC_FN_GROW(type, typename)                                 \
    int typename##_vec_grow(typename##_vec* vector, uint n_size) {  \
        if (n_size < vector->size) return -1;                       \
                                                                    \
        type* out = realloc(vector->arr, n_size);                   \
                                                                    \
        if (out == NULL) return -1;                                 \
                                                                    \
        free(vector->arr);                                          \
                                                                    \
        vector->size = n_size;                                      \
        vector->arr = out;                                          \
                                                                    \
        return 0;                                                   \
    }

#define VEC_FN_PROTO_ADD(type, typename)                            \
bool typename##_vec_add(typename##_vec* vector, type toAdd);

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
            free(vector->arr);                                      \
                                                                    \
            vector->size = new_size;                                \
            vector->arr = out;                                      \
        }                                                           \
        vector->arr[vector->pos++] = toAdd;                         \
                                                                    \
        return true;                                                \
    }

#define VEC_FN_PROTO_GET(type, typename)                            \
     type typename##_vec_get(typename##_vec* vector, uint index);

#define VEC_FN_GET(type, typename)                                  \
    type typename##_vec_get(typename##_vec* vector, uint index) {   \
        if (index >= vector->size) assert(false);                   \
                                                                    \
        return vector->arr[index];                                  \
    }

#define VEC_FN_PROTO_CREATE(type, typename)                         \
    typename##_vec typename##_vec_create(size_t init_size);

#define VEC_FN_CREATE(type, typename)                               \
    typename##_vec typename##_vec_create(size_t init_size) {        \
        type* arr = (type*) malloc(init_size * sizeof(type));       \
                                                                    \
        if (arr == NULL) {                                          \
            return (typename##_vec) {NULL, 0, -1};                  \
        }                                                           \
        return (typename##_vec) {arr, init_size, 0};                \
    }

#define VEC_FN_PROTO_DESTROY(type, typename)                        \
    void typename##_vec_destroy(typename##_vec* vector);

#define VEC_FN_DESTROY(type, typename)                              \
    void typename##_vec_destroy(typename##_vec* vector) {           \
        free(vector->arr);                                          \
        vector->size = -1;                                          \
        vector->pos = -1;                                           \
    }

#define VEC_FN_PROTO_STEAL(type, typename)                          \
    type* typename##_vec_steal(typename##_vec* vector);

#define VEC_FN_STEAL(type, typename)                                \
    type* typename##_vec_steal(typename##_vec* vector) {            \
        type* n_arr = (type*) malloc(vector->size * sizeof(type));  \
        if (n_arr == NULL) {                                        \
            return NULL;                                            \
        }                                                           \
                                                                    \
        type* temp = vector->arr;                                   \
        vector->arr = n_arr;                                        \
                                                                    \
        return temp;                                                \
    }

#define VEC_FN_PROTO_POP(type, typename)                            \
    type typename##_vec_pop(typename##_vec* vector);

#define VEC_FN_POP(type, typename)                                  \
    type typename##_vec_pop(typename##_vec* vector) {               \
        if (vector->pos == 0) return 0;                             \
                                                                    \
        vector->pos -= 1;                                           \
                                                                    \
        return vector->arr[vector->pos];                            \
    }

VEC_PROTO(char*, charp)
VEC_PROTO(char, char)

#endif //ATOMIC_VECTOR_H
