//
// Created by jamescoward on 11/11/2023.
//

#ifndef ATOMIC_ERRORS_H
#define ATOMIC_ERRORS_H

#include "Commons.h"

#include "SharedIncludes/Colours.h"
#include "SharedIncludes/Helper_String.h"

/* Structure of error code
 * |-------------|--------------|
 * -             0              +
 * ^^^^^^^^^^^       ^^^^^^^^^^^
 *     ERRORS           WARNINGS
 *            ^^^^^^^
 *            12 bits of
 *            semantics
 */

typedef struct errcode {
    uint16_t fatal: 1;
    uint16_t _UNUSED0 : 1;
    uint16_t _UNUSED1 : 1;
    uint16_t _UNUSED2 : 1;
    uint16_t _UNUSED3 : 1;
    uint16_t _UNUSED4 : 1;
    uint16_t _UNUSED5 : 1;
    uint16_t _UNUSED6 : 1;
    uint16_t _UNUSED7 : 1;
    uint16_t _UNUSED8 : 1;
    uint16_t _UNUSED9 : 1;
    uint16_t _UNUSED10: 1;

    uint16_t CODE_1_B: 1;
    uint16_t CODE_2_B: 1;
    uint16_t CODE_3_B: 1;
    uint16_t CODE_4_B: 1;

    int16_t code;
} errcode;

#define SUCCESS 0
#define FAIL    1

#define END     1   // found end, either end of parsing
#define ARGERR  2
#define ENOMEM  12

extern const char** const ERROR_CODE_STRINGS;
marked_string error_code_string(errcode err);

#define FATAL(c)      ((errcode){.fatal= true , .code= c})
#define ERROR(c)      ((errcode){.fatal= false, .code= -c})
#define ERRORC(code)  (-code)
#define WARN(c)       ((errcode){.code= c})
#define SUCC          ((errcode){.code= SUCCESS})

#define BASEERR 1024
#define LEXERR (BASEERR + 1)
#define LEXERR_MAX (LEXERR + 1024)
#define PARSERR (LEXERR_MAX + 1)

#define ERROR_BASE C_RED"ERROR"C_RST": "
#define WARN_BASE C_YLW"Warning"C_RST": "

#define INFO_BASE C_BLU"Info"C_RST": "
#define SUCC_BASE C_GRN"SUCCESS"C_RST": "

//https://gcc.gnu.org/onlinedocs/gcc/Variadic-Macros.html  -- gave error changed a bit
//and if the variable arguments are omitted or empty, the ‘##’ operator causes the preprocessor to remove the comma before it.
//Errros
#define PError(ErrorMsg, ...) printf(ERROR_BASE ErrorMsg , ##__VA_ARGS__)

#define ATOM_CT__CLI_ERR_ARG_NONE       "The ATOMIC compiler requires at least one argument (file to compile). Got: %d\n"
#define ATOM_CT__CLI_ERR_ARG_NULL       "Error with argv, null pointer in argument %d\n"

#define ATOM_CT__CLI_ERR_OPT_ARG        "Option \"%s\" require at least 1 argument\n"

//Warnings
#define PWarn(WarningMsg, ...) printf(WARN_BASE WarningMsg , ##__VA_ARGS__)

#define ATOM_CT__CLI_WRN_FLAG_INVALID   "Invalid flag given \"%s\"\n"
#define ATOM_CT__CLI_ERR_OPT_INVALID    "Invalid option given \"%s\"\n"

#define ATOM_CT__CLI_WRN_OPT_ARG_COUNT  "Option \"-%s\" only acts on 1 argument, ignoring others\n"
#define ATOM_CT__CLI_WRN_OPT_ARG_INVLD  "Option \"-%s\" given invalid argument \"%s\". Ignoring\n"

#endif //ATOMIC_ERRORS_H
