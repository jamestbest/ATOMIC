//
// Created by james on 19/12/23.
//

#ifndef ATOMIC_LEXERR_H
#define ATOMIC_LEXERR_H

#include "../Commons.h"
#include "../Errors.h"

extern Vector* llines;

typedef enum Lexerrors {
    LEXERR_UNKNOWN_SYMBOL,

    LEXERR_INT_INVALID_BASE,
    LEXERR_INT_INVALID_INT,
    LEXERR_INT_INVALID_DIGIT_FOR_BASE,

    LEXERR_FLOAT_TRAILING_DECIMAL,
    LEXERR_FLOAT_INVALID_FLOAT,

    LEXERR_COMMENT_MULTILINE_NO_END,

    LEXERR_PTR_OFFSET_OUT_OF_RANGE,

    LEXERR_EXPECTED_TYPE_AFTER_PTR_OFFSET,
} Lexerrors;

typedef enum Lexwarns {
    LEXWARN_INT_MISSING_BASE,
} Lexwarns;

void lexwarn(Lexwarns warnCode, Position pos, ...);
uint lexerr(Lexerrors errorCode, Position pos, ...);

//WARNINGS
#define ATOM_CT__LEX_WRN_INT_LIT_BASE_MISSING \
            "Found preceding 0 in INT literal, usually used to specify base of literal"     \
            " however it is followed by another digit not a base character (b/t/o/d/x)\n"


//ERRORS
#define ATOM_CT__LEXERR_UNKNOWN_SYMBOL \
            "Found invalid/unknown symbol `%c` (%u)\n"

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

#define ATOM_CT__LEXERR_PTR_OFFSET_OUT_OF_RANGE \
            "Pointer offset of %d is out of range (0, 0xFFFF), you may want to reconsider your design choice\n"

#define ATOM_CT__LEXERR_EXPECTED_TYPE_AFTER_PTR_OFFSET \
            "Expected to find a type (i4, n8, char, etc.) after the pointer qualifier `>` that was found after a type set `:`\n" \
            "Found token of type %s\n"

#endif // ATOMIC_LEXERR_H
