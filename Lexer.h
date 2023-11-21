//
// Created by james on 28/10/23.
//

#ifndef ATOMIC_LEXER_H
#define ATOMIC_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "SharedIncludes/Vector.h"
#include "SharedIncludes/Buffer.h"
#include "SharedIncludes/Helper_File.h"

#include "Tokens.h"

#define BUFF_SIZE 100

void lex(FILE* file, Token_vec* tokens, charp_vec* lines);
void line_to_tokens(Buffer* line, Token_vec* tokens);

#endif //ATOMIC_LEXER_H