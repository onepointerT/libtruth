// Copyright 2025 The OnePointer Authors.
//

#include <cstring>
#include <string>
#include <string_view>

#include "bool_base.hpp"

#include "boolexpr.hpp"





namespace truth {


BoolExpr::BoolExpr( const char* exprstr )
    :   std::string( exprstr )
{}
BoolExpr::BoolExpr( const std::string exprstr )
    :   std::string( exprstr )
{}
BoolExpr::BoolExpr( const std::string_view exprstr )
    :   std::string( exprstr.data() )
{}

BoolTerm BoolExpr::is() const {
    if ( compare( 0, 1, "&" ) == 0 || compare( 0, 1, "|" ) == 0 )
        return BoolTerm::OP;
    else if ( compare( 0, 1, "(" ) == 0 ) return BoolTerm::LBRACKET;
    else if ( compare( 0, 1, ")" ) == 0 ) return BoolTerm::RBRACKET;
    else if ( compare( 0, 1, "!" ) == 0 ) return BoolTerm::NEGATION;
    else if ( compare( 0, 1, " " ) == 0 && compare( length()-1, 1, " " ) == 0 )
        return BoolTerm::VARIABLE;
    else return BoolTerm::TERM;
}

void BoolExpr::operator=( const char* exprstr ) {
    swap( *(new std::string(exprstr)) );
}

bool BoolExpr::operator==( const std::string another ) {
    return ( (*this) == another && BoolExpr(another).is() == is() );
}
bool BoolExpr::operator!=( const std::string another ) {
    return ( (*this) != another && BoolExpr(another).is() != is() );
}


BoolExpr& BoolExpr::operator-=( const std::string substract ) {
    size_t spos = this->find_first_of( substract );
    if ( spos != this->npos ) {
        this->erase( spos, substract.length() );
    }
    return *this;
}


BoolExpr& BoolExpr::operator-( const std::string substract ) {
    size_t spos = this->find_first_of( substract );
    if ( spos != this->npos ) {
        this->erase( spos, substract.length() );
    }
    return *this;
}


std::string BoolExpr::find_prefix_before( const std::string expr ) const {
    size_t pos = this->find_first_of( expr );

    if ( pos == this->npos ) return "";
    return this->substr( 0, pos );
}


std::string BoolExpr::find_suffix_behind( const std::string expr ) const {
    size_t pos = this->find_first_of( expr );

    if ( pos == this->npos ) return "";
    return this->substr( pos + expr.length() + 1 );
}


size_t BoolExpr::find_first_of_all( const std::initializer_list<const char*> ils, size_t pos, size_t count ) const {

    std::vector< size_t > posvec;
    for ( std::string s : ils ) {
        posvec.emplace_back( this->find_first_of( s.c_str(), pos, count ) );
    }

    size_t smallest = pos;
    unsigned int idx = 0;
    for ( size_t p : posvec ) {
        if ( p == npos ) continue;
        for ( unsigned int i = idx+1; i < posvec.size(); i++ ) {
            if ( posvec[i] == npos ) continue;
            if ( p > posvec[i] ) smallest = posvec[i];
        }
        if ( smallest > p ) smallest = p;
        ++idx;
    }
    return smallest;
}


size_t BoolExpr::find_last_of_all( const std::initializer_list<const char*> ils, size_t pos, size_t count ) const {

    std::vector< size_t > posvec;
    for ( std::string s : ils ) {
        posvec.emplace_back( this->find_last_of( s.c_str(), pos, count ) );
    }

    size_t greatest = pos;
    unsigned int idx = 0;
    for ( size_t p : posvec ) {
        if ( p == npos ) continue;
        for ( unsigned int i = idx+1; i < posvec.size(); i++ ) {
            if ( posvec[i] == npos ) continue;
            if ( p < posvec[i] ) greatest = posvec[i];
        }
        if ( greatest < p ) greatest = p;
        ++idx;
    }
    return greatest;
}


size_t BoolExpr::rfind_first_of_all( const std::initializer_list<const char*> ils, size_t pos, size_t count ) const {

    std::vector< size_t > posvec;
    for ( std::string s : ils ) {
        posvec.emplace_back( this->rfind( s.c_str(), pos, count ) );
    }

    size_t first = pos;
    unsigned int idx = 0;
    for ( size_t p : posvec ) {
        if ( p == npos ) continue;
        for ( unsigned int i = idx+1; i < posvec.size(); i++ ) {
            if ( posvec[i] == npos ) continue;
            if ( p < posvec[i] ) first = posvec[i];
        }
        if ( first < p ) first = p;
        ++idx;
    }
    return first;
}


BoolExpr::expr_pair_t BoolExpr::splitAtFirstOperator( const BoolOperator::Type logic_operator ) const {
    size_t pos_lop = 0;
    if ( logic_operator == BoolOperator::ANY ) {
        pos_lop = this->find_first_of_all( { "&&", "||", ">>" } );
    } else {
        pos_lop = this->find_first_of( BoolOperator::string(logic_operator) );
    }
    
    if ( pos_lop == npos ) return { { nullptr, nullptr }, BoolOperator::UNKNOWN };

    std::string be1 = this->substr( 0, pos_lop-1 );
    std::string be2 = this->substr( pos_lop+2 );
    std::string opstr = this->substr( pos_lop, 2 );

    return { { new BoolExpr(be1), new BoolExpr(be2) }, BoolOperator::is(opstr) };
}


BoolExpr::expr_pair_t BoolExpr::splitAtLastOperator( const BoolOperator::Type logic_operator ) const {
    size_t pos_lop = 0;
    if ( logic_operator == BoolOperator::ANY ) {
        pos_lop = this->find_last_of_all( { "&&", "||", ">>" } );
    } else {
        pos_lop = this->find_last_of( BoolOperator::string(logic_operator) );
    }
    
    if ( pos_lop == npos ) return { { nullptr, nullptr }, BoolOperator::UNKNOWN };

    std::string be1 = this->substr( 0, pos_lop-1 );
    std::string be2 = this->substr( pos_lop+2 );
    std::string opstr = this->substr( pos_lop, 2 );

    return { { new BoolExpr(be1), new BoolExpr(be2) }, BoolOperator::is(opstr) };
}

BoolExpr::expr_pair_t BoolExpr::splitBehindFirstClosingTermBracket() const {

    size_t pos_cb = this->find_first_of( ')' );
    if ( pos_cb == npos ) return { { nullptr, nullptr }, BoolOperator::UNKNOWN };

    size_t pos_cb_op = this->find_first_of_all( { "&&", "||", ">>" }, pos_cb );
    if ( pos_cb_op == npos ) return { { new BoolExpr(substr( 0, pos_cb+1 )), nullptr }, BoolOperator::UNKNOWN };

    std::string opstr = this->substr( pos_cb_op, 2 );

    return { { new BoolExpr(substr( 0, pos_cb+1 )), new BoolExpr(substr( pos_cb_op+2 )) }, BoolOperator::is(opstr) };
}

BoolExpr::expr_pair_t BoolExpr::splitBeforeLastOpeningTermBracket() const {

    size_t pos_ob = this->find_last_of( '(' );
    if ( pos_ob == npos ) return { { nullptr, nullptr }, BoolOperator::UNKNOWN };

    size_t pos_ob_op = this->rfind_first_of_all( { "&&", "||", ">>" }, pos_ob );
    if ( pos_ob_op == npos ) return { { nullptr, new BoolExpr(substr( pos_ob )) }, BoolOperator::UNKNOWN };

    std::string opstr = this->substr( pos_ob_op, 2 );

    return { { new BoolExpr(substr( 0, pos_ob_op-1 )), new BoolExpr(substr( pos_ob )) }, BoolOperator::is(opstr) };
}

} // namespace truth



constexpr truth::BoolState operator++( const truth::BoolState state ) {
    if ( state == truth::LEFT ) return truth::OP1;
    else if ( state == truth::OP1 ) return truth::MID;
    else if ( state == truth::MID ) return truth::OP2;
    else return truth::RIGHT;
}


