//
// Created by jamescoward on 25/01/2024.
//

#include "Stack.h"

static bool verify_stack(Stack* stack);

bool verify_stack(Stack* stack) {
    if (!stack) return false;

    if (!stack->arr) return false;

    if (stack->elem_count == (size_t)-1 || stack->ptr == (size_t)-1) return false;

    return true;
}

Stack stack_create(size_t elem_count) {
    void** arr = malloc(sizeof(void*) * elem_count);

    if (!arr) return (Stack){NULL, -1, -1};

    Stack stack = (Stack){arr, 0, elem_count};

    return stack;
}

bool stack_resize(Stack* stack, size_t new_elem_count) {
    if (!verify_stack(stack)) return false;

    if (new_elem_count < stack->elem_count) return false;

    size_t new_size = new_elem_count * sizeof(void*);
    void** new_arr = realloc(stack->arr, new_size);

    if (!new_arr) return false;

    *stack = (Stack){new_arr, stack->ptr, new_elem_count};

    return true;
}

bool stack_push(Stack* stack, void* element) {
    if (!verify_stack(stack)) return false;

    if (stack->ptr == stack->elem_count) {
        size_t new_elem_count = (int)((double)stack->elem_count * 1.5);
        if (!stack_resize(stack, new_elem_count)) return false;
    }

    stack->arr[stack->ptr++] = element;

    return true;
}

void* stack_pop(Stack* stack) {
    if (!verify_stack(stack)) return false;

    if (stack->ptr == 0) return NULL;

    return stack->arr[--(stack->ptr)];
}

void stack_destroy(Stack* stack) {
    if (!verify_stack(stack)) return;

    free(stack->arr);

    *stack = (Stack){NULL, -1, -1};
}
