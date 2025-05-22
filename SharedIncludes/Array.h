//
// Created by jamescoward on 07/06/2024.
//

#ifndef ARRAY_H
#define ARRAY_H

#include <errno.h>
#include <stdlib.h>

#include "Commons.h"

#define MIN_ARRAY_SIZE 10

typedef struct {
    uint element_size;
    void* arr;
    size_t capacity;
    size_t pos;
} Array;

static const Array ARRAY_ERR;

Array arr_construct(const uint element_size, const uint min_element_count);
Array arr_create(const uint element_size);
bool arr_is_at_capacity(const Array* array);
void arr_resize(Array* array);
void arr_add(Array* array, const void* element);
void arr_add_dyn(Array* array, const void* element, const size_t element_size);
void arr_set(const Array* array, const size_t index, const void* element);
void arr_set_dyn(const Array* array, const size_t index, const void* element, const size_t element_size);
bool arr_remove(Array* array, const uint index);
void* arr_pop(Array* array);
void* arr_peek(const Array* array);
void* arr_ptr(const Array* array, const uint index);
void arr_destroy(Array* array);
void arr_sort(const Array* arr, int (*cmp_func)(const void* a, const void* b));
uint arr_search(const Array* array, const void* search_elem, int (*cmp_func)(const void* a, const void* b));

#define ARRAY_PROTO(type, typename)                                            \
    typedef struct {                                                           \
        uint element_size;                                                     \
        type* arr;                                                             \
        size_t capacity;                                                       \
        size_t pos;                                                            \
    } typename##Array;                                                         \
                                                                               \
    void typename##_arr_add(typename##Array* arr, const type element);         \
    type typename##_arr_get(const typename##Array* arr, const uint index);     \
    type* typename##_arr_ptr(const typename##Array* arr, const uint index);    \
    type typename##_arr_pop(typename##Array* arr);                             \
    type typename##_arr_peek(const typename##Array* arr);                      \
    typename##Array typename##_arr_construct(uint elem_s, uint elem_c);        \
    typename##Array typename##_arr_create();                                   \
    bool typename##_arr_is_at_capacity(const typename##Array* array);          \
    void typename##_arr_resize(typename##Array* array);                        \
    void typename##_arr_set(const typename##Array* array,                      \
        const size_t index, const type* element);                              \
    bool typename##_arr_remove(typename##Array* array, const uint index);      \
    void typename##_arr_destroy(typename##Array* array);                       \

#define ARRAY_ADD(type, typename)                                              \
    void typename##_arr_add(typename##Array* arr, const type element) {        \
         arr_add((Array*)arr, &element);                                       \
    }                                                                          \
                                                                               \
    type typename##_arr_get(const typename##Array* arr, const uint index) {    \
         return *(type*)arr_ptr((Array*)arr, index);                           \
    }                                                                          \
                                                                               \
    type* typename##_arr_ptr(const typename##Array* arr, const uint index) {   \
        return (type*)arr_ptr((Array*)arr, index);                             \
    }                                                                          \
                                                                               \
    type typename##_arr_pop(typename##Array* arr) {                            \
        return *(type*)arr_pop((Array*)arr);                                   \
    }                                                                          \
                                                                               \
    type typename##_arr_peek(const typename##Array* arr) {                     \
        return *(type*)arr_peek((Array*)arr);                                  \
    }                                                                          \
                                                                               \
    typename##Array typename##_arr_construct(const uint elem_s, uint elem_c) { \
        type* memory= malloc(elem_s * elem_c);                                 \
                                                                               \
        if (!memory) exit(ENOMEM);                                             \
                                                                               \
        const typename##Array arr= (typename##Array){                          \
            elem_s,                                                            \
            memory,                                                            \
            elem_c,                                                            \
            0                                                                  \
        };                                                                     \
                                                                               \
        return arr;                                                            \
    }                                                                          \
                                                                               \
    typename##Array typename##_arr_create() {                                  \
        return typename##_arr_construct(sizeof (type), MIN_ARRAY_SIZE);        \
    }                                                                          \
                                                                               \
    void typename##_arr_resize(typename##Array* array) {                       \
           return arr_resize((Array*) array);                                  \
    }                                                                          \
                                                                               \
    void typename##_arr_set(                                                   \
        const typename##Array* array,                                          \
        const size_t index,                                                    \
        const type* element                                                    \
    ) {                                                                        \
        return arr_set((Array*)array, index, (void*)element);                  \
    }                                                                          \
                                                                               \
    bool typename##_arr_remove(typename##Array* array, const uint index) {     \
        return arr_remove((Array*)array, index);                               \
    }                                                                          \
                                                                               \
    void typename##_arr_destroy(typename##Array* array) {                      \
        return arr_destroy((Array*)array);                                     \
    }                                                                          \
                                                                               \
    void typename##_arr_sort(                                                  \
        typename##Array* array, int (*cmp)(const void* a, const void* b)       \
    ) {                                                                        \
        return arr_sort((Array*)array, cmp);                                   \
    }                                                                          \
                                                                               \
    uint typename##_arr_search(                                                \
        typename##Array* array,                                                \
        const void* search_element,                                            \
        int (*cmp)(const void* a, const void* b)                               \
    ) {                                                                        \
        return arr_search((Array*)array, search_element, cmp);                 \
    }                                                                          \


#define ARRAY_CMP(type, typename, generic_cmp, comparable_element)             \
    int typename##_arr_cmp_i(                                                  \
            const void* a, const void* b                                       \
    ){                                                                         \
        return generic_cmp(                                                    \
            ((type*)a)->comparable_element,                                    \
            ((type*)b)->comparable_element                                     \
        );                                                                     \
    }                                                                          \
                                                                               \
    void typename##_arr_sort_i(                                                \
        typename##Array* array                                                 \
    ) {                                                                        \
        return arr_sort((Array*)array, typename##_arr_cmp_i);                  \
    }                                                                          \
                                                                               \
    int typename##_arr_search_cmp_i(                                           \
        const void* search_element,                                            \
        const void* array_element                                              \
    ) {                                                                        \
        const void* array_attribute=((type*)array_element)->comparable_element;\
        return generic_cmp(array_attribute, search_element);                   \
    }                                                                          \
                                                                               \
    uint typename##_arr_search_i(                                              \
        typename##Array* array,                                                \
        const void* search_element                                             \
    ) {                                                                        \
        return arr_search(                                                     \
            (Array*)array,                                                     \
            search_element,                                                    \
            typename##_arr_search_cmp_i                                        \
        );                                                                     \
    }                                                                          \

#define ARRAY_ADD_CMP(type, typename, generic_cmp, comparable_element)         \
    ARRAY_ADD(type, typename)                                                  \
    ARRAY_CMP(type, typename, generic_cmp, comparable_element)                 \

#define ARRAY_JOINT(type, typename)                                            \
    ARRAY_PROTO(type, typename)                                                \
    ARRAY_ADD(type, typename)                                                  \

#define ARRAY_JOINT_CMP(type, typename, generic_cmp, comparable_element)       \
    ARRAY_PROTO(type, typename)                                                \
    ARRAY_ADD(type, typename)                                                  \
    ARRAY_CMP(type, typename, generic_cmp, comparable_element)                 \

#endif //ARRAY_H
