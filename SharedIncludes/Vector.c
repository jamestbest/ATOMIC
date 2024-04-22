//
// Created by james on 07/11/23.
//

#include "Vector.h"

VEC_ADD(int, int)

bool vector_grow(Vector* vector, size_t new_size) {
    if (new_size < vector->size) return false;

    void** array = realloc(vector->arr, new_size * sizeof(void*));

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

VecRet vector_get(Vector* vector, size_t index) {
    //if you store NULL in the vector then you cannot do error checking, should this include an error code as well?
    if (vector == NULL) return (VecRet) {NULL, 1};

    if (index > vector->pos) return (VecRet) {NULL, 1};

    return (VecRet) {vector->arr[index], 0};
}

Vector vector_create(size_t size) {
    void* arr = malloc(size * sizeof(void*));

    if (!arr) return (Vector) {NULL, -1, -1};

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

VecRet vector_data_steal(Vector* vector) {
    if (vector == NULL) return (VecRet) {NULL, 1};

    Vector new_vec = vector_create(vector->size);

    if (new_vec.arr == NULL) return (VecRet) {NULL, 1};

    *vector = new_vec;

    return (VecRet) {vector->arr, 0};
}

VecRet vector_data_copy(Vector* vector) {
    Vector vec2 = vector_create(vector->size);

    if (vec2.arr == NULL) return (VecRet) {NULL, 1};

    memcpy(vec2.arr, vector->arr, vector->size * sizeof(void*));

    return (VecRet) {vec2.arr, 0};
}

Vector vector_copy(Vector* vector) {
    VecRet ret = vector_data_copy(vector);

    if (ret.retCode != 0) {
        return (Vector){NULL, -1, -1};
    }

    return (Vector){ret.data, vector->size, vector->pos};
}

VecRet vector_pop(Vector* vector) {
    if (vector == NULL) return (VecRet) {NULL, 1};

    if (vector->pos == 0) return (VecRet) {NULL, 1};

    return (VecRet) {vector->arr[--(vector->pos)], 0};
}
