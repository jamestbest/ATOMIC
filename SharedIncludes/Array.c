//
// Created by jamescoward on 07/06/2024.
//

#include "Array.h"
#include "../Errors.h"
#include <stdlib.h>
#include <string.h>

const static Array ARRAY_ERR = (Array) {
    .arr = NULL,
    .capacity = -1,
    .element_size = -1,
    .pos = -1,
};

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

// todo: refactor to make more clear that the input is the element_size and not the array size
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

void arr_add(Array* arr, const void* element) {
    if (arr_is_at_capacity(arr))
        arr_resize(arr);

    memcpy(&arr->arr[arr->pos++ * arr->element_size], element, arr->element_size);
}

// add to the array a value that is not the size of the array element size
void arr_add_dyn(Array* arr, const void* element, const size_t element_size) {
    if (arr_is_at_capacity(arr))
        arr_resize(arr);

    memset(&arr->arr[arr->pos * arr->element_size], 0, arr->element_size);
    memcpy(&arr->arr[arr->pos++ * arr->element_size], element, element_size);
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

void* arr_peek(const Array* arr) {
    if (arr->pos == 0) return NULL;

    return arr_get(arr, arr->pos - 1);
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