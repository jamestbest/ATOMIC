//
// Created by james on 07/11/23.
//

#include "Vector.h"

#include "../Errors.h"
#include "Helper_Math.h"

#include <malloc.h>
#include <string.h>

static const Vector VEC_ERR = (Vector){
    .arr = NULL,
    .capacity = -1,
    .pos = -1
};

Vector vector_create(const size_t element_count) {
    const size_t min_element_count = smax(element_count, MIN_VEC_SIZE);
    void** arr = malloc(min_element_count * sizeof (void*));

    if (!arr) return VEC_ERR;

    return (Vector) {
        .arr = arr,
        .capacity = min_element_count,
        .pos = 0
    };
}

bool vector_at_capacity(const Vector* vec) {
    return vec->pos == vec->capacity;
}

bool vector_verify(const Vector* vec) {
    return vec && vec->arr != NULL && vec->capacity != (size_t)-1 && vec->pos != (size_t)-1;
}

bool vector_resize(Vector* vec, const size_t new_element_count) {
    if (!vector_verify(vec)) return false;

    const size_t new_bytes = new_element_count * sizeof (void*);

    void* new_arr = realloc(vec->arr, new_bytes);

    if (!new_arr) return false;

    vec->capacity = new_element_count;
    vec->arr = new_arr;

    return true;
}

static size_t vector_resize_function(const size_t capacity) {
    return capacity << 1;
}

bool vector_add(Vector* vec, void* element) {
    if (!vector_verify(vec)) return false;

    if (vector_at_capacity(vec))
        vector_resize(vec, vector_resize_function(vec->capacity));

    vec->arr[vec->pos++] = element;

    return true;
}

VecRet vector_remove(Vector* vec, const uint index) {
    if (!vector_verify(vec)) return (VecRet){NULL, FAIL};

    const uint elements_right = vec->pos - index - 1;
    void* res = vec->arr[index];

    vec->pos--;

    if (elements_right == 0) return (VecRet){res, SUCCESS};

    memmove(&vec->arr[index], &vec->arr[index + 1], elements_right * sizeof (void*));

    return (VecRet){res, SUCCESS};
}

void* vector_remove_unsafe(Vector* vec, const uint index) {
    return vector_remove(vec, index).data;
}

VecRet vector_pop(Vector* vec) {
    return vector_remove(vec, vec->pos - 1);
}

void* vector_pop_unsafe(Vector* vec) {
    return vector_pop(vec).data;
}

VecRet vector_get(const Vector* vec, const uint index) {
    if (!vector_verify(vec)) return (VecRet){NULL, FAIL};

    if (index >= vec->pos) return (VecRet){NULL, FAIL};

    return (VecRet){vec->arr[index], SUCCESS};
}

void* vector_get_unsafe(const Vector* vec, const uint index) {
    if (!vector_verify(vec)) {
        return NULL;
    }

    return vec->arr[index];
}

void vector_destroy(Vector* vec) {
    if (!vec) return;

    free(vec->arr);

    *vec = (Vector){NULL, -1, -1};
}

void vector_disseminate_destruction(Vector* vec) {
    if (!vec->arr) goto vdd_destroy;
    for (uint i = 0; i < vec->pos; ++i) {
        free(vec->arr[i]);
    }

vdd_destroy:
    vector_destroy(vec);
}
