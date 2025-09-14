//
// Created by jamescoward on 09/09/2025.
//

#ifndef ATOMIC_GENERATORINTERNAL_H
#define ATOMIC_GENERATORINTERNAL_H

#include "Generator.h"

ARRAY_ADD(ByteStmt, ByteStmt)
ARRAY_ADD(Value, Value)

ARRAY_ADD_CMP(StringEntry, StringEntry, strcmp, string)

#endif // ATOMIC_GENERATORINTERNAL_H
