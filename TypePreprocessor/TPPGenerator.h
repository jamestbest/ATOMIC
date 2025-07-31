//
// Created by jamescoward on 10/06/2025.
//

#ifndef ATOMIC_TPPGENERATOR_H
#define ATOMIC_TPPGENERATOR_H

#include <Errors.h>
#include "TypePreprocessor.h"

errcode generate(FILE*
                     header_output,
                 FILE*
                     code_output,
                 const char*
                     header_file_name);

#endif // ATOMIC_TPPGENERATOR_H
