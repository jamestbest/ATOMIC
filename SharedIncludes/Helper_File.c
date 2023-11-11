//
// Created by jamescoward on 11/11/2023.
//

#include "Helper_File.h"

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