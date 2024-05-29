//
// Created by james on 19/12/23.
//

#ifndef ATOMIC_LEXERR_H
#define ATOMIC_LEXERR_H

#include "../Errors.h"

typedef enum Lexerrors {
    LEXERR_INT_INVALID_BASE,
    LEXERR_INT_INVALID_INT,
    LEXERR_INT_INVALID_DIGIT_FOR_BASE,

    LEXERR_FLOAT_TRAILING_DECIMAL,
    LEXERR_FLOAT_INVALID_FLOAT,

    LEXERR_COMMENT_MULTILINE_NO_END,
} Lexerrors;

typedef enum Lexwarns {
    LEXWARN_INT_MISSING_BASE,
} Lexwarns;

//WARNINGS
#define ATOM_CT__LEX_WRN_INT_LIT_BASE_MISSING \
            "Found preceding 0 in INT literal, usually used to specify base of literal"     \
            " however it is followed by another digit not a base character (b/t/o/d/x)\n"


//ERRORS
#define ATOM_CT__LEXERR_INT_INVALID_BASE \
            "Invalid base found for INT literal, found character \'%s\' after 0.\n"         \
            "Supported bases include binary (0b), trinary (0t), octonal (0o), denary (0d), and hex (0x)\n"

#define ATOM_CT__LEXERR_INT_INVALID_INT \
            "Invalid INT literal found, ATOM lang dictates that the INT literal ends here \'%s\' however " \
            "the last character found by strtoll is \'%s\'\n"

#define ATOM_CT__LEXERR_INT_INVALID_DIGIT_FOR_BASE \
            "Trailing digits found on a number that do not match the given base of \'base %d\' " \
            "Found trailing digit(s) starting at \'%s\'\n"

#define ATOM_CT__LEXERR_FLOAT_TRAILING_DECIMAL \
            "Float literals cannot end with a decimal point '.', remove or add a last digit e.g. 0\n"

#define ATOM_CT__LEXERR_FLOAT_INVALID_FLOAT \
            "Invalid FLOAT literal found, ATOM lang dictates that the FLOAT literal ends here \'%s\' however " \
            "the last character found by strtold is \'%s\'\n"

#define ATOM_CT__LEXERR_COMMENT_MULTILINE_NO_END \
            "Multiline comment started but does not end, reaches EOF. Place *¬ to end the comment\n"

#endif // ATOMIC_LEXERR_H
