// Copyright 2026 The OnePointer Authors.
//

#pragma once


#include "oneptr/ds_expr.hpp"
#include "boolexpr.hpp"
#include "bool_value_dict.hpp"


namespace truth {


template< class BT = BoolExpr >
class RegexDict
    :   public truth::BoolDict<BT>
{
public:
    BoolDict();
};


class Regex
    :   public oneptr::ds::ExprVarEnv
{
public:
    Regex();
};


} // namespace truth

