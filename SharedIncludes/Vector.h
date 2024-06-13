//
// Created by james on 07/11/23.
//

#ifndef ATOMIC_VECTOR_H
#define ATOMIC_VECTOR_H

#include "../Commons.h"

typedef unsigned int uint;

typedef struct {
    void** arr;
    size_t capacity;
    size_t pos;
} Vector;

typedef struct VecRet {
    void* data;
    uint retCode;
} VecRet;

bool vec_grow(Vector* vector, size_t new_size);
bool vec_add(Vector* vector, void* data);
void* vec_get(Vector* vector, size_t index);
Vector vec_create(size_t size);
void vec_destroy(Vector* vector);
void vec_disseminate_destruction(Vector* vector);
VecRet vec_data_steal(Vector* vector);
VecRet vec_data_copy(Vector* vector);
Vector vec_copy(Vector* vector);
void* vec_pop(Vector* vector);

#endif //ATOMIC_VECTOR_H
