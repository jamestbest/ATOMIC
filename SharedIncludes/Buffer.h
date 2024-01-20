//
// Created by james on 20/11/23.
//

#ifndef ATOMIC_BUFFER_H
#define ATOMIC_BUFFER_H

#include <stdlib.h>
#include <string.h>

#include "Helper_String.h"

#define BUFF_MIN 100

typedef unsigned int uint;

typedef struct Buffer {
    char* data;
    uint size;
    uint pos;
} Buffer;

Buffer buffer_create(uint size);
int buffer_resize(Buffer* buffer, uint size);
int buffer_concat(Buffer *buffer, char *to_add);
int buffer_nconcat(Buffer* buffer, char* to_add, u_int32_t d_size);
char* buffer_steal(Buffer* buffer, uint new_size);
void buffer_clear(Buffer* buffer);
void buffer_destroy(Buffer* buffer);

#endif //ATOMIC_BUFFER_H
