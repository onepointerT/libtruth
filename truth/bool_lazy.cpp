// Copyright 2025 The OnePointer Authors.
//



#include "bool_lazy.hpp"

#include "bool_algebraic_bind.hpp"



namespace truth {


truth_pair_t& get_new( AtomicBoolTerm* abt ) {
    AlgebraicBind* ab = new AlgebraicBind( abt->atomic_terms.first, *new BoolExpr(""), BoolOperator::UNKNOWN );
    BoolDict<BoolValue>& bd = *new BoolDict<BoolValue>();
    ab->fillDict( ab->value, bd );
    return *(new truth_pair_t{ atomic_bool_ptr(abt), bool_dict_ptr(&bd) });
}


BoolValue eval( const truth_pair_t& tp ) {
    return tp.first->eval( *(tp.second) );
}


} // namespace truth


