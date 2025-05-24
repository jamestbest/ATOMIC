//
// Created by jamescoward on 29/07/2024.
//

#ifndef MESSAGES_H
#define MESSAGES_H

#include <Errors.h>

errcode inform  (const char* message, ...);
errcode warning (const char* message, ...);
errcode error   (const char* message, ...);
errcode fatal   (const char* message, ...);

__attribute__((noreturn)) void usage(const char* message, ...);
__attribute__((noreturn)) void panic(const char* message, ...);

#endif //MESSAGES_H
