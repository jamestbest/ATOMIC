//
// Created by jamescoward on 25/01/2024.
//

#ifndef ATOMIC_STACK_H
#define ATOMIC_STACK_H

#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#define MIN_STACK_SIZE 10

typedef struct Stack {
    void** arr;
    size_t ptr; //points to the next free spot i.e. 1 above top of stack
    size_t elem_count;
} Stack;

Stack stack_create(size_t elem_count);
bool stack_resize(Stack* stack, size_t new_elem_count);
bool stack_push(Stack* stack, void* element);
void* stack_pop(Stack* stack);
void* stack_peek(Stack* stack);
void stack_destroy(Stack* stack);
bool stack_empty(Stack* stack);

#endif //ATOMIC_STACK_H
