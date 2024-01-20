//
// Created by james on 07/11/23.
//

#include "Vector.h"

VEC_ADD(char*, charp)

bool vector_grow(Vector* vector, size_t new_size) {
    if (new_size < vector->size) return false;

    void* array = realloc(vector->arr, new_size * sizeof(void*));

    if (array == NULL) return false;

    *vector = (Vector){array, new_size, vector->pos};

    return true;
}

bool vector_add(Vector* vector, void* data) {
    if (vector == NULL) return false;

    if (vector->pos == vector->size) {
        if (!vector_grow(vector, vector->size << 1)) return false; //double for now, shouldn't be in future
    }

    vector->arr[vector->pos++] = data;

    return true;
}

void* vector_get(Vector* vector, size_t index) {
    //if you store NULL in the vector then you cannot do error checking, should this include an error code as well?
    if (vector == NULL) return NULL;

    if (index > vector->pos) return NULL;

    return vector->arr[index];
}

Vector vector_create(size_t size) {
    void* arr = malloc(size * sizeof(void*));

    if (arr == NULL) return (Vector) {NULL, -1, -1};

    return (Vector){arr, size, 0};
}

void vector_destroy(Vector* vector) {
    if (vector == NULL) return;

    free(vector->arr);

    *vector = (Vector) {NULL, -1, -1};
}

void vector_disseminate_destruction(Vector* vector) {
    if (vector == NULL) return;
    for (uint i = 0; i < vector->pos; i++) {
        free(vector->arr[i]);
    }
    vector_destroy(vector);
}

void** vector_steal(Vector* vector) {
    if (vector == NULL) return NULL;

    void** ret = vector->arr;

    Vector new_vec = vector_create(vector->size);

    if (new_vec.arr == NULL) return NULL;

    *vector = new_vec;

    return ret;
}

void** vector_copy(Vector* vector) {
    Vector vec2 = vector_create(vector->size);

    if (vec2.arr == NULL) return NULL;

    memcpy(vec2.arr, vector->arr, vector->size * sizeof(void*));

    return vec2.arr;
}

void* vector_pop(Vector* vector) {
    if (vector == NULL) return NULL;

    if (vector->pos == 0) return NULL;

    return vector->arr[--(vector->pos)];
}
