//
// Created by jamescoward on 23/05/2025.
//

#ifndef ATOMIC_TOKERR_H
#define ATOMIC_TOKERR_H

#include "Tokens.h"

#include "SharedIncludes/Helper_Math.h"

void highlight_line_err(Position pos, char* line, uint min_pos_printout);
void highlight_line_start_and_error(Token* parent, Token* issue, const Vector* lines);

#endif // ATOMIC_TOKERR_H
