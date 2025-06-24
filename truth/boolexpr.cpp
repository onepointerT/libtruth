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


std::string BoolExpr::find_prefix_before( const std::string expr ) {
    size_t pos = this->find_first_of( expr );

    if ( pos == this->npos ) return "";
    return this->substr( 0, pos );
}


std::string BoolExpr::find_suffix_behind( const std::string expr ) {
    size_t pos = this->find_first_of( expr );

    if ( pos == this->npos ) return "";
    return this->substr( pos + expr.length() + 1 );
}



} // namespace truth



constexpr truth::BoolState operator++( const truth::BoolState state ) {
    if ( state == truth::LEFT ) return truth::OP1;
    else if ( state == truth::OP1 ) return truth::MID;
    else if ( state == truth::MID ) return truth::OP2;
    else return truth::RIGHT;
}


