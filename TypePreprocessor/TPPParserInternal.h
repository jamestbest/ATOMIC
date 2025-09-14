//
// Created by james on 01/08/24.
//

#ifndef ATOMIC_TPPPARSERINTERNAL_H
#define ATOMIC_TPPPARSERINTERNAL_H

#include "TypePreprocessor.h"
#include <string.h>

const char* TYPELIKE_TYPE_STRINGS[TL_TYPE_LIKE_TYPES_COUNT]= {
    [TL_TYPE_FIX]= "TYPE FIX",
    [TL_TYPE]= "TYPE"
};

const char* LRBuiltInTypesStrings[LRB_COUNT]= {
    [LRB_VARIABLE]= "VARIABLES",
    [LRB_LITERAL]= "LITERALS",
    [LRB_ALL]= "ALL"
};

VECTOR_ADD_CMP(TypeFixInfo , TypeFixInfo , strcmp , base.general_type)
VECTOR_ADD_CMP(TypeLikeInfo, TypeLikeInfo, strcmp , general_type)
ARRAY_ADD_CMP (CoercionRule, CoercionRule, int_cmp, left.cmpable)
ARRAY_ADD_CMP (OperatorInfo, OperatorInfo, strcmp , name)
ARRAY_ADD_CMP (AliasInfo   , AliasInfo   , strcmp , name)
ARRAY_ADD_CMP (OperandInfo , OperandInfo , strcmp , operator->name)
ARRAY_ADD_CMP (LRValueData , LRValueData , int_cmp, cmpable)

ARRAY_ADD    (uint        , uint)

#endif // ATOMIC_TPPPARSERINTERNAL_H
