//
// Created by james on 01/08/24.
//

#ifndef ATOMIC_TPPPARSERINTERNAL_H
#define ATOMIC_TPPPARSERINTERNAL_H

#include "TypePreprocessor.h"

ARRAY_JOINT_CMP(TypeFixInfo , TypeFixInfo , strcmp, name        )
ARRAY_JOINT_CMP(TypeInfo    , TypeInfo    , strcmp, general_type)
ARRAY_JOINT_CMP(OperatorInfo, OperatorInfo, strcmp, name        )
ARRAY_JOINT_CMP(AliasInfo   , AliasInfo   , strcmp, name        )
ARRAY_JOINT    (OperandInfo , OperandInfo)

ARRAY_ADD      (uint        , uint)

#endif // ATOMIC_TPPPARSERINTERNAL_H
