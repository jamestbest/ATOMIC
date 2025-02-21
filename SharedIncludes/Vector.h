//
// Created by james on 07/11/23.
//

#ifndef ATOMIC_VECTOR_H
#define ATOMIC_VECTOR_H

#include "../Commons.h"

#define MIN_VEC_SIZE 10

typedef unsigned int uint;

typedef struct {
    void** arr;
    size_t pos;
    size_t capacity;
} Vector;

static const Vector VEC_ERR;

typedef struct VecRet {
    void* data;
    uint retCode;
} VecRet;

Vector vector_create(size_t element_count);
bool vector_at_capacity(const Vector* vec);
bool vector_verify(const Vector* vec);
bool vector_resize(Vector* vec, size_t new_element_count);
bool vector_add(Vector* vec, void* element);
VecRet vector_remove(Vector* vec, uint index);
void* vector_remove_unsafe(Vector* vec, uint index);
VecRet vector_pop(Vector* vec);
void* vector_pop_unsafe(Vector* vec);
VecRet vector_get(const Vector* vec, uint index);
void* vector_get_unsafe(const Vector* vec, uint index);
void vector_destroy(Vector* vec);
void vector_disseminate_destruction(Vector* vec);


#endif //ATOMIC_VECTOR_H
