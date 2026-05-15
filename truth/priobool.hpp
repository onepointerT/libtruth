// Copyright 2026 The OnePointer Authors.
//

#pragma once

#include "boolexpr_value.hpp"
#include "boolop.hpp"
#include "oneptr/priorizedmap.hpp"
#include "oneptr/priorizedsortingkey.hpp"

#include <utility>


namespace truth {
namespace priobool {


class PriorizedBool
    :   public truth::BoolExprValue
    ,   public oneptr::SortingKey< truth::BoolOperator >
{
public:
    typedef std::pair< PriorizedBool*, PriorizedBool* > sorted_result_t;

    PriorizedBool( const double priorizment, const std::string categories, const std::string key
                 , const std::string exprstr = "", truth::BoolValue::Type value = BoolValue::None );

    std::partial_ordering operator<=>( const PriorizedBool& another ) const;

    static const sorted_result_t getSorted( PriorizedBool& one, PriorizedBool& another );
};


using Map = oneptr::PriorizedMap< PriorizedBool, BoolOperator >;


class PriorizedMap
    :   public truth::priobool::Map
{
public:
    PriorizedMap();
};


} // namespace priobool
} // namespace truth
