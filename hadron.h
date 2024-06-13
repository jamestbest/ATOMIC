#ifndef HADRON_LIBRARY_H
#define HADRON_LIBRARY_H

#include <stdbool.h>

#ifndef uint
typedef unsigned int uint;
#endif

extern bool HADRON_EXPAND;
extern bool HADRON_RETAIN;
extern bool HADRON_VERBOSE;

void* hadron_malloc(uint bytes, char* file_name, uint line_number, const char* func_sig);
void* hadron_realloc(void* ptr, uint new_bytes, char* file_name, uint line_number, const char* func_sig);
void hadron_free(void* ptr, char* file_name, uint line_number, const char* func_sig);
void hadron_verify(const bool require_empty);
void hadron_verify_two(void);

#define malloc(bytes) hadron_malloc((bytes), __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define realloc(ptr, bytes) hadron_realloc(ptr, bytes, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define free(ptr) hadron_free(ptr, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#endif //HADRON_LIBRARY_H
