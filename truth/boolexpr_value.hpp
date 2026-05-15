// Copyright 2025 The OnePointer Authors.
//

#pragma once


#include "bool_value.hpp"
#include "boolexpr.hpp"




/// @brief The namespace of libtruth
namespace truth {


class BoolExprValue
    :   public truth::BoolExpr
    ,   public truth::BoolValue
{
public:
    BoolExprValue( const std::string boolexpr, BoolValue::Type value = BoolValue::None );
};


} // namespace truth


