//
// Created by jamescoward on 29/07/2024.
//

#include "Messages.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "SharedIncludes/Colours.h"
#include "SharedIncludes/Helper_String.h"
#include "Errors.h"

int inform(const char* message, ...) {
    putz(C_BLU"INFO: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    return EXIT_SUCCESS;
}

int warning(const char* message, ...) {
    putz(C_MGN"WARNING: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    return EXIT_SUCCESS;
}

int error(const char* message, ...) {
    putz(C_RED"ERROR: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    return EXIT_FAILURE;
}

__attribute__((noreturn)) void usage(const char* message, ...) {
    putz(C_RED"USAGE: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

 __attribute__((noreturn)) void panic(const char* message, ...) {
    putz(C_RED"PANIC: "C_RST);

    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);

    exit(EXIT_FAILURE);
}
