//
// Created by james on 07/11/23.
//

#ifndef ATOMIC_VECTOR_H
#define ATOMIC_VECTOR_H

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

typedef unsigned int uint;

typedef enum TYPE_ASSERTION { //used to assert
    CHAR_P,
    TOKEN_P,
    NODE,
    CORRUPT_DATA,
} TYPE_ASSERTION;

typedef struct Vector {
    void** arr;
    size_t size;
    size_t pos;
    TYPE_ASSERTION type;
} Vector;

typedef struct VecRet {
    void* data;
    uint retCode;
} VecRet;

bool vector_grow(Vector* vector, size_t new_size);
bool vector_add(Vector* vector, void* data);
VecRet vector_get(Vector* vector, size_t index);
Vector vector_create(size_t size, TYPE_ASSERTION type);
void vector_destroy(Vector* vector);
void vector_disseminate_destruction(Vector* vector);
VecRet vector_data_steal(Vector* vector);
VecRet vector_data_copy(Vector* vector);
Vector vector_copy(Vector* vector);
VecRet vector_pop(Vector* vector);

#define VEC_ADD(type, typename)                                     \
    VEC_FN_ADD(type, typename)                                      \
    VEC_FN_GET(type, typename)                                      \
    VEC_FN_CREATE(type, typename)                                   \
    VEC_FN_GROW(type, typename)                                     \
    VEC_FN_DESTROY(type, typename)                                  \
    VEC_FN_STEAL(type, typename)                                    \
    VEC_FN_POP(type, typename)
//    VEC_FN_DISSEMINATE_DESTRUCTION(type, typename)


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
//  VEC_FN_PROTO_DISSEMINATE_DESTRUCTION(type, typename)


#define VEC_FN_PROTO_GROW(type, typename)                           \
    int typename##_vec_grow(typename##_vec* vector, uint n_size);

#define VEC_FN_GROW(type, typename)                                 \
    int typename##_vec_grow(typename##_vec* vector, uint n_size) {  \
        if (n_size < vector->size) return -1;                       \
                                                                    \
        type* out = realloc(vector->arr, n_size * sizeof(type));    \
                                                                    \
        if (out == NULL) return -1;                                 \
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
            int out = typename##_vec_grow(vector, new_size);        \
                                                                    \
            if (out == -1) {                                        \
                return false;                                       \
            }                                                       \
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

#define VEC_FN_PROTO_DISSEMINATE_DESTRUCTION(type, typename)        \
    void typename##_vec_disseminate_destruction(typename##_vec*vec);

#define VEC_FN_DISSEMINATE_DESTRUCTION(type, typename)              \
    void typename##_vec_disseminate_destruction(typename##_vec*vec){\
        for (uint i = 0; i < vec->pos; i++) {                       \
            free(vec->arr[i]);                                      \
        }                                                           \
        typename##_vec_destroy(vec);                                \
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

//cannot return 0/NULL on pos = 0 as the return type can change
//there must be a way around this
#define VEC_FN_POP(type, typename)                                  \
    type typename##_vec_pop(typename##_vec* vector) {               \
                                                                    \
        vector->pos -= 1;                                           \
                                                                    \
        return vector->arr[vector->pos];                            \
    }

VEC_PROTO(char*, charp)

#endif //ATOMIC_VECTOR_H
