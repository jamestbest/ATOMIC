//
// Created by jamescoward on 25/01/2024.
//

#include "Errors.h"

#include "SharedIncludes/Helper_Math.h"

const char** const ERROR_CODE_STRINGS= (const char*[]){
    [SUCCESS]= "SUCCESS",
    [FAIL]= "GENERAL FAILURE"
};

const char FATAL_STRING[]= "FATAL";

marked_string error_code_string(errcode err) {
    // todo make this function work with so far non-existent errcode extensions (only works with fatal atm)
    bool is_fatal= err.fatal;

    char* general_string= ERROR_CODE_STRINGS[err.code];

    const int aux_count= is_fatal;

    if (aux_count == 0) return (marked_string){.str= general_string, .is_heap= false};

    const size_t size=
        strlen(general_string) +
        (is_fatal ? sizeof FATAL_STRING : 0) +
        1 + 2 + // 1 for \0 + 2 for ()
        2 * (max(aux_count - 1, 0)); // `, ` for each aux
    char* const ret= malloc(size);

    sprintf(ret, "%s(%s)", general_string, FATAL_STRING);

    return (marked_string){.str= ret, .is_heap= true};
}
