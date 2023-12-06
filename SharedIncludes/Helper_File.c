//
// Created by jamescoward on 11/11/2023.
//

#include "Helper_File.h"

FILE* open_file(const char* cwd, const char* filename, const char* mode) {
    FILE* fp = fopen(filename, mode);

    if (fp != NULL) return fp;

    const char* path = get_path(cwd, filename);

    fp = fopen(path, mode);

    free((char*) path);

    return fp;
}

char* get_dir(char* file) {
    // /dir/dir2/dir3/file

    int loc = find_last(file, path_sep_c);

    uint length = loc;

    char* dir = malloc((length + 1) * sizeof(char));

    memcpy(dir, file, length);
    dir[length] = '\0';

    return dir;
}

char* get_path(const char* dir, const char* file) {
    uint l1 = len(dir);
    uint l2 = len(file);

    char* ret = malloc((l1 + l2 + 1) * sizeof(char));

    memcpy(ret, dir, l1);
    memcpy(&ret[l1], file, l2);

    ret[l1 + l2] = '\0';

    return ret;
}

bool get_line(FILE* file, Buffer* buffer) {
    uint pos;
    char temp_buff[BUFF_MIN];

    temp_buff[0] = '\0';

    buffer_clear(buffer);

    do {
        if (feof(file) != 0) return buffer->pos != 0;

        char* fres = fgets(temp_buff, BUFF_MIN, file);

        if (fres == NULL) {
            return pos != 0;
        }

        pos = buffer->pos;

        int res = buffer_concat(buffer, temp_buff, BUFF_MIN);

        if (res != 0) return false;

    } while (strchr(&buffer->data[pos], '\n') == NULL);

    return true;
}
