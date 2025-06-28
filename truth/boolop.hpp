// Copyright 2025 The OnePointer Authors.
//

#pragma once



#include <cstddef>
#include <cstdint>

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
