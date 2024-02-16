//
// Created by jamescoward on 25/01/2024.
//

#include "Errors.h"

void highlight_line_err(Position pos, const char* line) {
    int position_length = print_position(pos);
    printf("|\t\t%s", line);

    if (position_length < 0) goto skip_position_offset;
    for (uint i = 0; i < (uint)position_length; i++) {
        putchar(' ');
    }

    skip_position_offset:
    printf(" \t\t");

    for (uint i = 1; i <= pos.end_col; i++) {
        if (i < pos.start_col) putchar(' ');
        else printf(C_RED"^");
    }
    puts(C_RST);
}
