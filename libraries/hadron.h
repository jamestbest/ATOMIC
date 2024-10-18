//
// Created by jamescoward on 24/08/2024.
//

// PUBLIC facing C file with defines for malloc, realloc, reallocarray, calloc, free

#ifndef HADRON_H
#define HADRON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>

#include <math.h>

void* HADRON__MALLOC(size_t bytes, const char* file_name, const char* func_sig, const uint32_t line_number);
void* HADRON__REALLOC(void* ptr, size_t bytes, const char* file_name, const char* func_sig, const uint32_t line_number);
void HADRON__FREE(void* ptr, const char* file_name, const char* func_sig, const uint32_t line_number);
void* HADRON__CALLOC(const size_t member_count, const size_t member_size, const char* file_name, const char* func_sig, const uint32_t line_number);

#define malloc(size) HADRON__MALLOC(size, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define realloc(ptr, new_size) HADRON__REALLOC(ptr, new_size, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define calloc(members_count, size) HADRON__CALLOC(members_count, size, __FILE__, __PRETTY_FUNCTION__, __LINE__)
#define free(ptr) HADRON__FREE(ptr, __FILE__, __PRETTY_FUNCTION__, __LINE__)

extern bool HADRON__FLAG_VERBOSE;                              // VBOSE
extern bool HADRON__FLAG_NO_KEEP_AFTER_FREE;                   // NOKAF
extern bool HADRON__FLAG_DEBUG;                                // DEBUG
extern bool HADRON__FLAG_DISPLAY_AS_YOU_GO;                    // DAYGO
extern bool HADRON__FLAG_DISPLAY_NON_ERROR_ALLOCATIONS;        // DNERA
extern bool HADRON__FLAG_LOG_TO_FILE;                          // LOGTF

int hadron_cleanup();
int hadron_verify();

#endif //HADRON_H
