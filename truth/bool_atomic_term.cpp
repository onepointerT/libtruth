// Copyright 2025 The OnePointer Authors.
//

#include <cstring>
#include <string>
#include <string_view>

#include "bool_atomic_term.hpp"

#include "bool_base.hpp"





namespace truth {



AtomicBoolTerm::AtomicBoolTerm( expr_t bool_expr )
    :   atomic_expr( bool_expr )
    ,   atomic_terms( *(BoolType::find_terms( bool_expr, *(new BoolType::bool_expression_t("", "")) ).first) )
{}
AtomicBoolTerm::AtomicBoolTerm( const atomic_term_t& bool_expr )
    :   atomic_expr( bool_expr.first )
    ,   atomic_terms( atomic_terms_t{ bool_expr.first, new oneptr::queue< atomic_term_t >() } )
{
    atomic_terms.second->emplace_front( bool_expr );
}
AtomicBoolTerm::AtomicBoolTerm( const atomic_terms_t& bool_expr )
    :   atomic_expr( bool_expr.first )
    ,   atomic_terms( bool_expr )
{}

bool AtomicBoolTerm::eval( const BoolDict<BoolValue> args ) {
    if ( BoolType::eval_bool( &this->atomic_terms, args ) == truth_true ) return true;
    return false;
}


} // namespace truth

