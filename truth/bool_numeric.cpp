// Copyright 2025 The OnePointer Authors.
//

#include "bool_numeric.hpp"

#include <vector>


namespace truth {


NumericOperator::Type NumericOperator::is( const std::string str ) {
    if ( str == "==" ) return NumericOperator::EQUAL;
    else if ( str == ">" ) return NumericOperator::GREATER;
    else if ( str == "<" ) return NumericOperator::SMALLER;
    else if ( str == "!=" ) return NumericOperator::UNEQUAL;
    else if ( str == ">=" ) return NumericOperator::GREATER_EQUAL;
    else if ( str == "<=" ) return NumericOperator::SMALLER_EQUAL;
    else return NumericOperator::UNKNOWN; 
}


const std::string NumericOperator::string( const NumericOperator::Type opt ) {
    if ( opt == NumericOperator::EQUAL ) return " == ";
    else if ( opt == NumericOperator::GREATER ) return " > ";
    else if ( opt == NumericOperator::SMALLER ) return " < ";
    else if ( opt == NumericOperator::UNEQUAL ) return " != ";
    else if ( opt == NumericOperator::GREATER_EQUAL ) return " >= ";
    else if ( opt == NumericOperator::SMALLER_EQUAL ) return " <= ";
    else return "";
}


size_t NumericOperator::find_next_of( const std::initializer_list<const char*> strs, const std::string str, const size_t pos, const size_t _count ) {

    std::vector<size_t> posvec;
    for ( std::string s : strs ) {
        size_t spos = str.find_first_of( s.c_str(), pos, _count );
        if ( spos != s.npos ) posvec.push_back( spos );
    }

    size_t smallest = pos;
    for ( size_t sz : posvec ) {
        if ( sz < smallest ) smallest = sz;
    }

    return smallest;
}

size_t NumericOperator::find_next_operator( const std::string str, const size_t pos, const size_t _count ) {
    return NumericOperator::find_next_of( { "==", "!=", ">", "<", ">=", "<=" }, str, pos, _count );
}


const NumericVariable& NumericOperator::lookup( const std::string varname, const NumericDict<>& nd ) {
    return nd.getValue( varname.c_str() );
}


bool NumericOperator::eval( const double lhs, const NumericOperator::Type nop, const double rhs ) {
    if ( nop == NumericOperator::EQUAL ) return lhs == rhs;
    else if ( nop == NumericOperator::GREATER ) return lhs > rhs;
    else if ( nop == NumericOperator::SMALLER ) return lhs < rhs;
    else if ( nop == NumericOperator::UNEQUAL ) return lhs != rhs;
    else if ( nop == NumericOperator::GREATER_EQUAL ) return lhs >= rhs;
    else if ( nop == NumericOperator::SMALLER_EQUAL ) return lhs <= rhs;
    else return false;
}

bool NumericOperator::eval( const std::string lhs, const NumericOperator::Type nop, const std::string rhs, const NumericDict<>& nd ) {
    return NumericOperator::eval( NumericOperator::lookup(lhs, nd), nop, NumericOperator::lookup(rhs, nd) );
}

bool NumericOperator::eval( const std::string lhs, const NumericOperator::Type nop, const double rhs, const NumericDict<>& nd ) {
    return NumericOperator::eval( NumericOperator::lookup(lhs, nd), nop, rhs );
}

bool NumericOperator::eval( const double lhs, const NumericOperator::Type nop, const std::string rhs, const NumericDict<>& nd ) {
    return NumericOperator::eval( lhs, nop, NumericOperator::lookup(rhs, nd) );
}

bool NumericOperator::eval( const NumericVariable& lhs, const NumericOperator::Type nop, const NumericVariable& rhs, const NumericDict<>& nd ) {
    if ( lhs.value.vname.length() > 0 && lhs.value.vname.length() > 0 )
        return NumericOperator::eval( lhs.value.vname, nop, rhs.value.vname, nd );
    else if ( lhs.value.vname.length() > 0 )
        return NumericOperator::eval( lhs.value.vname, nop, rhs.value.v, nd );
    else if ( lhs.value.vname.length() > 0 )
        return NumericOperator::eval( lhs.value.v, nop, rhs.value.vname, nd );
    else return NumericOperator::eval( lhs.value.v, nop, rhs.value.v );
}



void NumericTerm::parse() {
    term_pair_t tpt = NumericTerm::parse_term( m_term );

    variable_left = *tpt.first.first;
    variable_right = *tpt.first.second;
    op = tpt.second;
}


NumericTerm::term_pair_t NumericTerm::parse_term( const std::string str ) {

    size_t pos_first_op = NumericOperator::find_next_operator( str );
    if ( pos_first_op == str.npos ) return { { new NumericVariable(0.0), new NumericVariable(0.0) }, NumericOperator::UNKNOWN };

    std::string opstr = str.substr( pos_first_op, str.find_first_of( ' ', pos_first_op ) - pos_first_op - 1 );
    std::string varname1 = str.substr( 0, pos_first_op - 2 );
    std::string varname2 = str.substr( str.find_first_of( ' ', pos_first_op ) + 1 );

    return { {new NumericVariable(varname1), new NumericVariable(varname2) }, NumericOperator::is(opstr) };
}


NumericTerm::NumericTerm( const std::string term )
    :   m_term( term )
    ,   variable_left( *new NumericVariable(0.0) )
    ,   variable_right( *new NumericVariable(0.0) )
    ,   op( NumericOperator::UNKNOWN )
{
    this->parse();
}

bool NumericTerm::eval() const {
    return NumericOperator::eval( variable_left, op, variable_right );
}

bool NumericTerm::eval( const NumericDict<NumericVariable>& nd ) const {
    return NumericOperator::eval( variable_left, op, variable_right, nd );
}

const std::string NumericTerm::string() const {
    return variable_left.value.string() + NumericOperator::string(op) + variable_right.value.string();
}

const std::string NumericTerm::term() const {
    return m_term;
}

} // namespace truth
