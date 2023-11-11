//
// Created by jamescoward on 11/11/2023.
//

#ifndef ATOMIC_HELPER_FILE_H
#define ATOMIC_HELPER_FILE_H

#include <stdlib.h>
#include <string.h>

#include "Helper_String.h"

typedef unsigned int uint;

#ifdef _WIN32
#define path_sep_c '\\'
#define path_sep_s "\\"
#else
#define path_sep_c '/'
#define path_sep_s "/"
#endif

char* get_dir(char* file);
char* get_path(const char* dir, const char* file);

#endif //ATOMIC_HELPER_FILE_H
