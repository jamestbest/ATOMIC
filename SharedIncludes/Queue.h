//
// Created by jamescoward on 25/01/2024.
//

#ifndef ATOMIC_QUEUE_H
#define ATOMIC_QUEUE_H

#include <stdint.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#define MIN_QUEUE_SIZE 10

typedef struct Queue {
    void** arr;
    size_t front; //points to the element that would be popped()
    size_t back;  //points to the next free spot i.e. where push() would place the data
    size_t elem_count;
} Queue;

bool queue_resize(Queue* queue, size_t new_elem_count);
Queue queue_create(size_t elements);
bool queue_push(Queue* queue, void* element);
void* queue_pop(Queue* queue);
void queue_destroy(Queue* queue);

#endif //ATOMIC_QUEUE_H
