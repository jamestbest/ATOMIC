//
// Created by jamescoward on 02/06/2024.
//

#include "Lexerr.h"

char* lexerr_process_char(char a, char buff[2]) {
    if (a == '\n') return "\\n";
    if (a == '\t') return "\\t";

    buff[0] = a;
    return buff;
}

void highlight_current_line_err(Position pos) {
    highlight_line_err(pos, llines->arr[pos.start_line - 1], -1);
}

uint lexerr(const Lexerrors errorCode, const Position pos, ...) {
    va_list args;

    va_start(args, pos);

    char buff1[2] = {'f', '\0'};
    char buff2[2] = {'f', '\0'};

    printf(C_RED"LEXERR: "C_RST);

    bool print_current_line = true;

    //the line we are on is still in its buffer
    switch (errorCode) {
        case LEXERR_INT_INVALID_INT: {
            char* atom_end = lexerr_process_char(*va_arg(args, char*), buff1);
            char* strtoll_end = lexerr_process_char(*va_arg(args, char*), buff2);
                    printf(ATOM_CT__LEXERR_INT_INVALID_INT,
                           atom_end,
                           strtoll_end);
            break;
        }
        case LEXERR_INT_INVALID_BASE: {
            char* end_char = lexerr_process_char(*va_arg(args, char*), buff1);
            printf(ATOM_CT__LEXERR_INT_INVALID_BASE,
                   end_char);
            break;
        }
        case LEXERR_INT_INVALID_DIGIT_FOR_BASE: {
            int base = va_arg(args, int);
            char* start_char = lexerr_process_char(*va_arg(args, char*), buff1);
            printf(ATOM_CT__LEXERR_INT_INVALID_DIGIT_FOR_BASE,
                   base,
                   start_char);
            break;
        }
        case LEXERR_FLOAT_TRAILING_DECIMAL: {
            printf(ATOM_CT__LEXERR_FLOAT_TRAILING_DECIMAL);
            break;
        }
        case LEXERR_FLOAT_INVALID_FLOAT: {
            char* atom_end = lexerr_process_char(*va_arg(args, char*), buff1);
            char* strtold_end = lexerr_process_char(*va_arg(args, char*), buff2);
            printf(ATOM_CT__LEXERR_FLOAT_INVALID_FLOAT,
                   atom_end,
                   strtold_end);
            break;
        }
        case LEXERR_COMMENT_MULTILINE_NO_END: {
            printf(ATOM_CT__LEXERR_COMMENT_MULTILINE_NO_END);
            print_current_line = false;
            break;
        }
        case LEXERR_PTR_OFFSET_OUT_OF_RANGE: {
            int offset = va_arg(args, int);
            printf(ATOM_CT__LEXERR_PTR_OFFSET_OUT_OF_RANGE, offset);
            break;
        }
        case LEXERR_EXPECTED_TYPE_AFTER_PTR_OFFSET: {
            TPToken* t = va_arg(args, TPToken*);
            printf(ATOM_CT__LEXERR_EXPECTED_TYPE_AFTER_PTR_OFFSET,
                   get_token_type_string(t->type));
            break;
        }
    }

    if (print_current_line) highlight_current_line_err(pos);
    else {
        print_position(pos);
        printf("\n");
    }

    va_end(args);

    return errorCode;
}

void lexwarn(Lexwarns warnCode, Position pos, ...) {
    va_list args;

    va_start(args, pos);

    //the line we are on is still in its buffer
    switch (warnCode) {
        case LEXWARN_INT_MISSING_BASE:
            break;
    }
    highlight_current_line_err(pos);
}
