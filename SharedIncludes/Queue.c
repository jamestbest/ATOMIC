//
// Created by jamescoward on 25/01/2024.
//

#include "Queue.h"

static bool queue_resize_circular(Queue* queue, size_t new_elem_count, size_t new_size);
static bool queue_resize_flat(Queue* queue, size_t new_elem_count, size_t new_size);

static bool verify_queue(Queue* queue);

bool verify_queue(Queue* queue) {
    if (!queue) return false;

    if (!queue->arr) return false;

    if (
        queue->elem_count == (size_t)-1     ||
        queue->front == (size_t)-1          ||
        queue->back == (size_t)-1
        ) return false;

    return true;
}

bool queue_resize_circular(Queue* queue, size_t new_elem_count, size_t new_size) {
    /*
     *   [elem, elem, elem, elem, ----, ----, ----, ----, elem, elem]
     *                            ^back                   ^front
     *      Should atm only be triggered when back == front, but it's still the same premise
     */

    size_t elements_left = queue->back;
    size_t elements_right = queue->elem_count - queue->front;

    void** new_arr = malloc(new_size);

    if (!new_arr) return false;

    size_t size_left = elements_left * sizeof(void*);
    size_t size_right = elements_right * sizeof(void*);

    memcpy(new_arr, &queue->arr[queue->front], size_right);
    memcpy(&new_arr[elements_right], queue->arr, size_left);

    free(queue->arr);

    *queue = (Queue){new_arr, 0, elements_left + elements_right, new_elem_count};

    return true;
}

bool queue_resize_flat(Queue* queue, size_t new_elem_count, size_t new_size) {
    void** new_arr = malloc(new_size);
    if (!new_arr) return false;

    size_t elem_count = queue->back - queue->front;
    size_t copy_size = sizeof(void*) * elem_count;
    memcpy(new_arr, &queue->arr[queue->front], copy_size);

    free(queue->arr);

    *queue = (Queue){new_arr, 0, elem_count, new_elem_count};

    return true;
}

bool queue_resize(Queue* queue, size_t new_elem_count) {
    if (!verify_queue(queue)) return false;

    if (new_elem_count < queue->elem_count) return false;

    size_t new_size = new_elem_count * sizeof(void*);

    if (queue->front >= queue->back) return queue_resize_circular(queue, new_elem_count, new_size);
    else return queue_resize_flat(queue, new_elem_count, new_size);
}

Queue queue_create(size_t elements) {
    void** arr = malloc(elements * sizeof(void*));

    if (!arr) return (Queue){NULL, -1, -1, -1};

    return (Queue){arr, 0, 0,elements};
}

bool queue_push(Queue* queue, void* element) {
    if (!verify_queue(queue)) return false;

    if (queue->front == queue->back) {
        size_t new_elem_count = (int)((double)queue->elem_count * 1.5);
        if (!queue_resize(queue, new_elem_count)) return false;
    }

    queue->arr[queue->back++ % queue->elem_count] = element;

    return true;
}

void* queue_pop(Queue* queue) {
    if (!verify_queue(queue)) return NULL;

    if (queue->front == queue->back) return NULL;

    return queue->arr[queue->front++];
}

void queue_destroy(Queue* queue) {
    if (!verify_queue(queue)) return;

    free(queue->arr);

    *queue = (Queue){NULL, -1, -1, -1};
}
