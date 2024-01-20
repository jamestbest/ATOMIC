//
// Created by james on 20/11/23.
//

#include "Buffer.h"

Buffer buffer_create(uint size) {
    char* data = malloc(size * sizeof(char));

    if (data == NULL) {
        return (Buffer) {NULL, -1, -1};
    }

    return (Buffer) {data, size, 0};
}

int buffer_resize(Buffer* buffer, uint new_size) {
    char* new_data = realloc(buffer->data, new_size);

    if (new_data == NULL) {
        return -1;
    }

    buffer->data = new_data;
    buffer->size = new_size;

    return 0;
}

int buffer_nconcat(Buffer* buffer, char* to_add, u_int32_t d_size) {
    if (buffer->pos + d_size + 1 >= buffer->size) {
        uint new_size = buffer->size + d_size + 1;

        new_size += BUFF_MIN;
        new_size &= ~BUFF_MIN;

        int res = buffer_resize(buffer, new_size);

        if (res != 0) return res;
    }

    if (buffer->pos > 0 && buffer->data[buffer->pos - 1] == '\0') {
        buffer->pos--;
    }

    memcpy(&buffer->data[buffer->pos], to_add, d_size + 1);

    buffer->pos += d_size;
    buffer->data[buffer->pos++] = 0;

    return 0;
}

int buffer_concat(Buffer *buffer, char *to_add) {
    uint size_to_add = len(to_add);
    return buffer_nconcat(buffer, to_add, size_to_add);
}

char* buffer_steal(Buffer* buffer, uint new_size) {
    //can no longer assume that buffer.data is not NULL as a theft from buffer can leave realloced result as null
    //alt - could return NULL from steal - ?
    //multiple thefts would pick up issue as NULL returned but not the last

    char* new_data = malloc(new_size);

    char* ret = buffer->data;

    *buffer = (Buffer){new_data, new_size, 0};

    return ret;
}

void buffer_clear(Buffer* buffer) {
    buffer->pos = 0;
}

void buffer_destroy(Buffer* buffer) {
    free(buffer->data);

    *buffer = (Buffer){NULL, -1, -1};
}
