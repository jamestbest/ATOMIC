//
// Created by jamescoward on 25/01/2024.
//

#ifndef ATOMIC_QUEUE_H
#define ATOMIC_QUEUE_H

#include "../Commons.h"


#define MIN_QUEUE_SIZE 10

typedef struct Queue {
    void** arr;
    size_t front; //points to the element that would be popped()
    size_t back; // points to the last element in the queue i.e. back + 1 is the place for push()
    size_t capacity;
    bool is_empty;
} Queue;

bool queue_resize(Queue* queue, size_t new_elem_count);
Queue queue_create(size_t elements);
bool queue_push(Queue* queue, void* element);
void* queue_pop(Queue* queue);
void queue_destroy(Queue* queue);
unsigned int queue_current_count(Queue* queue);

#endif //ATOMIC_QUEUE_H
