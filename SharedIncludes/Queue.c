//
// Created by jamescoward on 25/01/2024.
//

// [[todo]] I have fucked this code
//      i am too tired to think
//      please fix

#include "Queue.h"

#include <stdlib.h>
#include <string.h>

static bool queue_resize_circular(Queue* queue, size_t new_elem_count, size_t new_size);
static bool queue_resize_flat(Queue* queue, size_t new_elem_count, size_t new_size);

static bool verify_queue(Queue* queue);

static bool queue_full(Queue* queue);
static bool queue_at_one(Queue* queue);

bool verify_queue(Queue* queue) {
    if (!queue) return false;

    if (!queue->arr) return false;

    if (
            queue->capacity == (size_t)-1 ||
        queue->front == (size_t)-1 ||
        queue->back == (size_t)-1
        ) return false;

    return true;
}

bool queue_resize_circular(Queue* queue, size_t new_elem_count, size_t new_size) {
    /*
     *   [elem, elem, elem, elem, ----, ----, ----, ----, elem, elem]
     *                      ^back                         ^front
     *      Should atm only be triggered when queue is full i.e. back + 1 == front (% size),
     *       but it's still the same premise
     */

    size_t elements_left = queue->back + 1;
    size_t elements_right = queue->capacity - queue->front;

    void** new_arr = malloc(new_size);

    if (!new_arr) return false;

    size_t size_left = elements_left * sizeof(void*);
    size_t size_right = elements_right * sizeof(void*);

    memcpy(new_arr, &queue->arr[queue->front], size_right);
    memcpy(&new_arr[elements_right], queue->arr, size_left);

    free(queue->arr);

    *queue = (Queue){new_arr, 0, elements_left + elements_right, new_elem_count,
                     queue->is_empty};

    return true;
}

bool queue_resize_flat(Queue* queue, size_t new_elem_count, size_t new_size) {
    void** new_arr = malloc(new_size);
    if (!new_arr) return false;

    size_t elem_count = queue->back - queue->front + 1;
    size_t copy_size = sizeof(void*) * elem_count;
    memcpy(new_arr, &queue->arr[queue->front], copy_size);

    free(queue->arr);

    *queue = (Queue){new_arr, 0, elem_count, new_elem_count,
                     queue->is_empty};

    return true;
}

bool queue_resize(Queue* queue, size_t new_elem_count) {
    if (!verify_queue(queue)) return false;

    if (new_elem_count < queue->capacity) return false;

    size_t new_size = new_elem_count * sizeof(void*);

    if (queue->front > queue->back) return queue_resize_circular(queue, new_elem_count, new_size);
    else return queue_resize_flat(queue, new_elem_count, new_size);
}

Queue queue_create(size_t elements) {
    void** arr = malloc(elements * sizeof(void*));

    if (!arr) return (Queue){NULL, -1, -1, -1, true};

    return (Queue){arr, 0, 0,elements, true};
}

bool queue_full(Queue* queue) {
    return queue->front == (queue->back + 1) % queue->capacity;
}

bool queue_at_one(Queue* queue) {
    return (queue->front == queue->back) && !queue->is_empty;
}

bool queue_push(Queue* queue, void* element) {
    if (!verify_queue(queue)) return false;

    if (queue_full(queue)) {
        size_t new_elem_count = (int)((double)queue->capacity * 1.5);
        if (!queue_resize(queue, new_elem_count)) return false;
    }

    if (queue->is_empty) {
        queue->is_empty = false;
        queue->arr[(queue->back)] = element;
        return true;
    }

    queue->arr[(++queue->back) % queue->capacity] = element;

    return true;
}

void* queue_pop(Queue* queue) {
    if (!verify_queue(queue)) return NULL;

    if (queue->is_empty) return NULL;

    if (queue_at_one(queue)) {
        queue->is_empty = true;
        return queue->arr[queue->front];
    }

    return queue->arr[queue->front++];
}

void queue_destroy(Queue* queue) {
    if (!verify_queue(queue)) return;

    free(queue->arr);

    *queue = (Queue){NULL, -1, -1, -1, true};
}

unsigned int queue_current_count(Queue* queue) {
    if (queue->is_empty) return 0;

    if (queue_at_one(queue)) return 1;

    if (queue->back < queue->front) {
        return queue->capacity - (queue->front - queue->back - 1);
    }

    return queue->back - queue->front + 1;
}
