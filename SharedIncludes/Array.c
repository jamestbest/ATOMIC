//
// Created by jamescoward on 07/06/2024.
//

#include "Array.h"
#include "../Errors.h"

Array arr_construct(const uint element_size, const uint min_element_count) {
    void* memory = malloc(element_size * min_element_count);

    if (!memory) exit(ENOMEM);

    const Array vec = (Array) {
        element_size,
        memory,
        min_element_count,
        0
    };

    return vec;
}

Array arr_create(const uint element_size) {
    return arr_construct(element_size, MIN_VEC_SIZE);
}

bool arr_is_at_capacity(const Array* vec) {
    return vec->pos == vec->capacity;
}

void arr_resize(Array* vec) {
    const uint new_capacity = vec->capacity << 1;

    char* new_memory = realloc(vec->arr, new_capacity * vec->element_size);

    if (!new_memory) exit(ENOMEM);

    vec->arr = new_memory;
    vec->capacity = new_capacity;
}

void arr_add(Array* vec, const void* element) {
    if (arr_is_at_capacity(vec))
        arr_resize(vec);

    memcpy(&vec->arr[vec->pos++ * vec->element_size], element, vec->element_size);
}

bool arr_remove(Array* arr, const uint index) {
    if (index >= arr->pos) return false;

    const uint elements_right = arr->pos - index - 1;

    arr->pos--;

    void* dst = &arr->arr[index * arr->element_size];
    const void* src = &arr->arr[(index + 1) * arr->element_size];

    memmove(dst, src, elements_right);

    return true;
}

void* arr_pop(Array* arr) {
    if (arr->pos == 0) return NULL;

    const uint index = arr->pos - 1;
    void* element = arr_get(arr, index);
    arr_remove(arr, index);

    return element;
}

void* arr_get(const Array* arr, const uint index) {
    if (index >= arr->pos) return NULL;

    return &arr->arr[index * arr->element_size];
}

void arr_destroy(Array* vec) {
    free(vec->arr);

    *vec = (Array) {
        -1,
        NULL,
        -1,
        -1
    };
}