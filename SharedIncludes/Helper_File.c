//
// Created by jamescoward on 11/11/2023.
//

#include <unistd.h>
#include <assert.h>
#include "Helper_File.h"

FILE* validate_file(const char* filename, const char* mode) {
    //need some way to allow checks for a file's existence
    uint mode_length = len(mode);

    if (access(filename, F_OK) != 0) {
        return fopen(filename, mode);
    }

    for (uint i = 0; i < mode_length; i++) {
        int dec;
        switch (mode[i]) {
            case 'r':
                dec = R_OK;
                break;
            case 'w':
                dec = W_OK;
                break;
            default:
                assert(false);
        }

        int res = access(filename, dec);

        if (res != 0) return NULL;
    }

    FILE* fp = fopen(filename, mode);

    return fp;
}

FILE* open_file(const char* cwd, const char* filename, const char* mode) {
    FILE* fp = validate_file(filename, mode);

    if (fp != NULL) return fp;

    const char* path = get_path(cwd, filename);

    fp = validate_file(path, mode);

    free((char*) path);

    return fp;
}

char* get_dir(char* file) {
    // /dir/dir2/dir3/file = /dir/dir2/dir3/

    int loc = find_last(file, path_sep_c);

    uint length = loc + 1;

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
    uint pos = 0;
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

        int res = buffer_concat(buffer, temp_buff);

        if (res != 0) return false;

    } while (strchr(&buffer->data[pos], '\n') == NULL);

    return true;
}
