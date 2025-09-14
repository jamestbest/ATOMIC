//
// Created by jamescoward on 10/09/2025.
//

#ifndef INC_235_VM_H
#define INC_235_VM_H

#include "SharedIncludes/Array.h"
#include "Generator/Generator.h"

ARRAY_PROTO_CMP(StringEntry, StringEntry, strcmp, string)

int run(ByteStmtArray stmts, StringEntryArray functions);

#endif //INC_235_VM_H
