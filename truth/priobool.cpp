// Copyright 2026 The OnePointer Authors.
//

#include "priobool.hpp"


namespace truth {
namespace priobool {


PriorizedBool::PriorizedBool( const double priorizment, const std::string categories, const std::string key
                            , const std::string exprstr, truth::BoolValue::Type value )
    :   truth::BoolExprValue( exprstr, value )
    ,   oneptr::SortingKey<BoolOperator>( priorizment, categories, key, BoolOperator::UNKNOWN )
{}

std::partial_ordering PriorizedBool::operator<=>( const PriorizedBool& another ) const {
    return this->default_compare_all( another );
}

const sorted_result_t PriorizedBool::getSorted( PriorizedBool& one, PriorizedBool& another ) {
    if ( one <=> another ) return { one, another };
    else return { another, one };
}


PriorizedMap::PriorizedMap()
    :   priobool::Map()
{}


} // namespace priobool
} // namespace truth
