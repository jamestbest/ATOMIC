//
// Created by jamescoward on 29/07/2024.
//

#ifndef MESSAGES_H
#define MESSAGES_H

void inform(const char* message, ...);
void error(const char* message, ...);
void panic(const char* message, ...);
void warning(const char* message, ...);

#endif //MESSAGES_H
