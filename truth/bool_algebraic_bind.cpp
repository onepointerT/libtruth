// Copyright 2025 The OnePointer Authors.
//

#include "bool_base.hpp"

#include "bool_algebraic_bind.hpp"





namespace truth {

AlgebraicBind::AlgebraicBind( const std::string expr_left, const std::string expr_right, const BoolOperator::Type op )
    :   value( *(new bindval_t{ {*(new BoolExpr(expr_left)), *(new BoolExpr(expr_right))}, op }) )
    ,   truth{ *(new BoolValue( BoolValue::Unknown )), *(new BoolValue( BoolValue::Unknown )) }
{}

AlgebraicBind::AlgebraicBind( BoolExpr& expr_left, BoolExpr& expr_right, const BoolOperator::Type op )
    :   value( *(new bindval_t{ {expr_left, expr_right}, op }) )
    ,   truth{ *(new BoolValue( BoolValue::Unknown )), *(new BoolValue( BoolValue::Unknown )) }
{}

AlgebraicBind::AlgebraicBind( bindval_t& bindval )
    :   value( bindval )
    ,   truth{ *(new BoolValue( BoolValue::Unknown )), *(new BoolValue( BoolValue::Unknown )) }
{}


bool AlgebraicBind::eval() const {
    return BoolOperator::eval( truth.first, value.second, truth.second );
}


const std::vector< AlgebraicSmallTerm* >& small_terms( const BoolExpr& expr ) {

    std::vector< AlgebraicSmallTerm* >* vast = new std::vector< AlgebraicSmallTerm* >();

    BoolType::small_term_queue_t* stq = BoolType::find_operator_in_expr( expr );

    for ( const oneptr::queue_element< BoolType::small_term_t >* qe : { stq->front(), stq->back() } ) {
        AlgebraicSmallTerm* ast = new AlgebraicSmallTerm( *(qe->get()) );
        vast->push_back( ast );
    }

    return *vast;
}

const std::vector< AlgebraicSmallTerm* >& AlgebraicBind::small_terms_left() const {
    return AlgebraicBind::small_terms( this->value.first.first );
}

const std::vector< AlgebraicSmallTerm* >& AlgebraicBind::small_terms_right() const {
    return AlgebraicBind::small_terms( this->value.first.second );
}

BoolDict<BoolValue>& AlgebraicBind::fillDict( const bindval_t expr, BoolDict<BoolValue>& bd ) {
    const std::vector< AlgebraicSmallTerm* >& left_expr_vec = small_terms( expr.first.first );
    const std::vector< AlgebraicSmallTerm* >& right_expr_vec = small_terms( expr.first.second );

    for ( AlgebraicSmallTerm* ast : left_expr_vec ) {
        ast->addToDict( bd );
    }
    for ( AlgebraicSmallTerm* ast : right_expr_vec ) {
        ast->addToDict( bd );
    }

    return bd;
}


AlgebraicSmallTerm::AlgebraicSmallTerm( const std::string var1, const std::string var2, const BoolOperator::Type op, const BoolExpr& bexpr )
    :   term{ {var1, var2}, op }
    ,   expr( bexpr )
    ,   value( BoolValue::Unknown )
{}


AlgebraicSmallTerm::AlgebraicSmallTerm( const small_term_t& stt )
    :   term{ {stt.first.first, stt.first.second}, stt.second }
    ,   expr( *(new BoolExpr(stt.first.first + BoolOperator::string( stt.second ) + stt.first.second)) )
    ,   value( BoolValue::Unknown )
{}


AlgebraicSmallTerm& AlgebraicSmallTerm::new_small_term( const BoolExpr& algebraic_logic_expr ) {
    BoolType::small_term_queue_t* stq = BoolType::find_operator_in_expr( algebraic_logic_expr );
    return *(new AlgebraicSmallTerm( *(stq->front()->get()) ));
}

BoolDict<BoolValue>& AlgebraicSmallTerm::fillDict( const varnames_t varnames, BoolDict<BoolValue>& bd ) {
    
    bool has_var1 = false;
    bool has_var2 = false;
    for ( BoolDict<BoolValue>::const_iterator cit : { bd.begin(), bd.end() } ) {
        if ( cit->first == varnames.first ) has_var1 = true;
        if ( cit->first == varnames.second ) has_var2 = true;
    }
    if ( ! has_var1 ) bd.at( varnames.first ) = BoolValue::Unknown;
    if ( ! has_var2 ) bd.at( varnames.second ) = BoolValue::Unknown;

    return bd;
}


BoolDict< BoolValue >& AlgebraicSmallTerm::addToDict( BoolDict< BoolValue >& bd ) {
    return this->fillDict( term.first, bd );
}



} // namespace truth

