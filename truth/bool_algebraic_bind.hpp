// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "bool_value_dict.hpp"
#include "boolexpr.hpp"
#include "boolop.hpp"

namespace truth {


class AlgebraicBind {
public:
    /// @brief The basic type with a left and a right expression that can be matched with operators.
    typedef std::pair< BoolExpr, BoolExpr > expr_t;
    /// @brief The basic type that matches and `expr_t` with a `BoolOperator`.
    typedef std::pair< const expr_t, const BoolOperator::Type > bindval_t;

    /// @brief 
    /// @param expr 
    /// @return 
    static bool evalbool( const bindval_t expr, const BoolDict<BoolValue>& bd );
    /// @brief 
    /// @param expr 
    /// @param bd 
    /// @return 
    static BoolDict<BoolValue>& fillDict( const bindval_t expr, BoolDict<BoolValue>& bd );
};


class AlgebraicSmallTerm {
public:
    /// @brief 
    typedef std::pair< const std::string, const std::string > varnames_t;
    /// @brief 
    typedef std::pair< const varnames_t, const BoolOperator::Type > small_term_t;

    /// @brief 
    small_term_t term;
    /// @brief 
    const BoolExpr expr;

    /// @brief 
    AlgebraicSmallTerm();
    /// @brief 
    /// @param algebraic_logic_expr 
    AlgebraicSmallTerm( const BoolExpr algebraic_logic_expr );
};

} // namespace truth


