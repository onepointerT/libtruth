// Copyright 2025 The OnePointer Authors.
//

#pragma once



#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "bool_value_dict.hpp"



namespace truth {


/// @brief The basic functionality class that can handle all supported operators, when they have one or two operands.
class BoolOperator {
public:
    /// @brief All known operators of this library "libtruth"
    enum Type : uint8_t {
        AND = 0,
        OR = 1,
        NEGATION = 2,
        IMPLIES = 3,
        UNKNOWN = 8,
        ANY = 7
    };

    static Type is( const std::string op ) {
        return ( op == "&&" ? Type::AND : ( op == "||" ? Type::OR : ( op == ">>" ? Type::IMPLIES : ( op == "!" ? Type::NEGATION : Type::UNKNOWN ) ) ) );
    }

    static const std::string string( const BoolOperator::Type op ) {
        if ( op == Type::AND ) return " && ";
        else if ( op == Type::OR ) return " || ";
        else if ( op == Type::IMPLIES ) return " >> ";
        else if ( op == Type::NEGATION ) return " ! ";
        else if ( op == Type::ANY ) return "{ &&, ||, >> }";
        else return "";
    }

    static size_t find_next_of( const std::initializer_list<const char*> strs, const std::string str, const size_t pos = 0, const size_t _count = std::string::npos ) {

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
    static size_t find_next_operator( const std::string str, const size_t pos = 0, const size_t _count = std::string::npos ) {
        return BoolOperator::find_next_of( { "&&", "||", ">>" }, str, pos, _count );
    }

    static bool eval( const bool lhs, const BoolOperator::Type op, const bool rhs ) {
        if ( op == Type::UNKNOWN ) return false;
        else if ( op == Type::AND ) return lhs && rhs;
        else if ( op == Type::OR ) return lhs || rhs;
        else if ( op == Type::IMPLIES ) return !(lhs && !rhs) && rhs;
        else if ( op == Type::NEGATION ) return !lhs;
        else return false;
    }
};




} // namespace truth
/*
/// @brief The `AND` operator of this library
#define AND truth::BoolOperator::operator&&
/// @brief The `OR` operator of this library
#define OR truth::BoolOperator::operator||
/// @brief The `IMPLIES` operator of this library
#define IMPLIES truth::BoolOperator::operator>>
/// @brief The `NOT` operator of this library
#define NOT truth::BoolOperator::operator!*/
