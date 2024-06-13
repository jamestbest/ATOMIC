//
// Created by james on 07/11/23.
//

#include "Vector.h"

bool vec_grow(Vector* vector, size_t new_size) {
    if (new_size < vector->capacity) return false;

    void** array = realloc(vector->arr, new_size * sizeof(void*));

    if (array == NULL) return false;

    *vector = (Vector){array, new_size, vector->pos};

    return true;
}

static bool vec_at_capacity(const Vector* arr) {
    return arr->pos == arr->capacity;
}

bool vec_add(Vector* vector, void* data) {
    if (vector == NULL) return false;

    if (vec_at_capacity(vector)) {
        if (!vec_grow(vector, vector->capacity << 1)) return false; //double for now, shouldn't be in future
    }

    vector->arr[vector->pos++] = data;

    return true;
}

void* vec_get(Vector* vector, size_t index) {
    //if you store NULL in the vector then you cannot do error checking, should this include an error code as well?
    if (vector == NULL) return NULL;

    if (index >= vector->pos) return NULL;

    return vector->arr[index];
}

Vector vec_create(size_t size) {
    void* arr = malloc(size * sizeof(void*));

    if (!arr) return (Vector) {NULL, -1, -1};

    return (Vector){arr, size, 0};
}

void vec_destroy(Vector* vector) {
    if (vector == NULL) return;

    free(vector->arr);

    *vector = (Vector){NULL, -1, -1};
}

void vec_disseminate_destruction(Vector* vector) {
    if (vector == NULL) return;
    for (uint i = 0; i < vector->pos; i++) {
        free(vector->arr[i]);
    }
    vec_destroy(vector);
}

VecRet vec_data_steal(Vector* vector) {
    if (vector == NULL) return (VecRet) {NULL, 1};

    Vector new_vec = vec_create(vector->capacity);

    if (new_vec.arr == NULL) return (VecRet) {NULL, 1};

    *vector = new_vec;

    return (VecRet) {vector->arr, 0};
}

VecRet vec_data_copy(Vector* vector) {
    Vector vec2 = vec_create(vector->capacity);

    if (vec2.arr == NULL) return (VecRet) {NULL, 1};

    memcpy(vec2.arr, vector->arr, vector->capacity * sizeof(void*));

    return (VecRet) {vec2.arr, 0};
}

Vector vec_copy(Vector* vector) {
    VecRet ret = vec_data_copy(vector);

    if (ret.retCode != 0) {
        return (Vector){NULL, -1, -1};
    }

    return (Vector){ret.data, vector->capacity, vector->pos};
}

void* vec_pop(Vector* vector) {
    if (vector == NULL) return NULL;

    if (vector->pos == 0) return NULL;

    return vector->arr[--(vector->pos)];
}
