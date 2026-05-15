// Copyright 2025 The OnePointer Authors.
//


#include "boolexpr_value.hpp"



namespace truth {


BoolExprValue::BoolExprValue( const std::string boolexpr, BoolValue::Type value )
    :   truth::BoolExpr( boolexpr )
    ,   truth::BoolValue( value )
{}


} // namespace truth

