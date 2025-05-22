//
// Created by jamescoward on 29/07/2024.
//

#ifndef MESSAGES_H
#define MESSAGES_H

int inform  (const char* message, ...);
int warning (const char* message, ...);
int error   (const char* message, ...);

__attribute__((noreturn)) void usage(const char* message, ...);
__attribute__((noreturn)) void panic(const char* message, ...);

#endif //MESSAGES_H
