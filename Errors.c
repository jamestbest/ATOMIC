//
// Created by jamescoward on 25/01/2024.
//

#include "Errors.h"

static void highlight_line(Position pos, const char* line, const char* colour);

void highlight_line_multiple(const char* line, const Position displayPos, char* colours[], ...) {
    if (colours[0] == NULL) return;

    // for each colour a position is expected!
    va_list args;

    va_start(args, colours);

    uint colPos = 1;
    uint colourPos = 0;

    int position_length = print_position(displayPos);
    printf("|    %s", line);

    for (uint i = 0; i < (uint)position_length; i++) {
        putchar(' ');
    }

    printf("|    ");

    // todo: refactor; changes added but not written well
    while (colours[colourPos] != NULL) {
        Position pos = va_arg(args, Position);

        while (colPos < pos.start_col) {putchar(' '); colPos++;};

        putz(colours[colourPos++]);

        while (colPos <= pos.end_col) {
            if (colPos == pos.start_col || colPos == pos.end_col) putchar('^');
            else putchar('~');
            colPos++;
        };

        putz(C_RST);
    }
    va_end(args);

    putchar('\n');
}

void highlight_line(Position pos, const char* line, const char* colour) {
    // todo: this won't work if there is a special printout in the error printout e.g. \r then the col position will be offset by 1 after
    int position_length = print_position(pos);
    putz("|    ");
    putz_santitize(line);
    newline();

    if (position_length < 0) goto skip_position_offset;
    for (uint i = 0; i < (uint)position_length; i++) {
        putchar(' ');
    }

skip_position_offset:
    printf("|    ");

    putz(colour);
    for (uint i = 1; i <= pos.end_col; i++) {
        if (i < pos.start_col) putchar(' ');
        else putchar(i == pos.start_col || i == pos.end_col ? '^' : '~');
    }
    puts(C_RST);
}

void highlight_line_err(Position pos, const char* line) {
    highlight_line(pos, line, C_RED);
}

void highlight_line_warning(Position pos, const char* line) {
    highlight_line(pos, line, C_MGN);
}

void highlight_line_info(Position pos, const char* line) {
    highlight_line(pos, line, C_BLU);
}

void highlight_line_start_and_error(Token* parent, Token* issue, const Vector* lines) {
    if (!parent && !issue)
        assert(false);

    Position startPos = parent->pos;
    char* startLine = lines->arr[startPos.start_line - 1];

    if (!issue) {
        highlight_line_info(startPos, startLine);
        return;
    }

    Position errorPos = issue->pos;
    char* errorLine = lines->arr[errorPos.start_line - 1];

    bool sameLine = startPos.start_line == errorPos.start_line && startPos.end_line == errorPos.end_line;

    if (!sameLine) {
        highlight_line_info(startPos, startLine);
        highlight_line_err(errorPos, errorLine);

        return;
    }

    highlight_line_multiple(startLine, errorPos, (char*[]){C_BLU, C_RED, NULL}, startPos, errorPos);
}
