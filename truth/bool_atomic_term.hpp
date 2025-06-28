// Copyright 2025 The OnePointer Authors.
//

#pragma once



#include <cstddef>
#include <deque>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "boolexpr.hpp"
#include "bool_value_dict.hpp"




/// @brief The namespace of libtruth
namespace truth {


/// @brief An atomic and thus elemental algebraic bool term.
/// @note Atomic terms are small and comprehensivly cumultative terms of an algebraic, even algebraic-logicly, greater expression.
class AtomicBoolTerm
{

public:
    /// @brief atomic bool expression like "a&&b || b"
    expr_t atomic_expr;
    /// @brief Hierarichal evaluation synonym for evaluation by operator and brackets
    const atomic_terms_t atomic_terms;
    
    /// @brief Constructor
    /// @param bool_expr A `BoolExpr`
    AtomicBoolTerm( expr_t bool_expr );
    /// @brief Constructor
    /// @param bool_expr An `atomic_term_t` that categorizes the algebraic expression
    AtomicBoolTerm( const atomic_term_t& bool_expr );
    /// @brief Constructor
    /// @param bool_expr An `atomic_terms_t` that contains the `expr_t` and the evaluated queue
    ///    with all elemental terms of the algebraic expression
    AtomicBoolTerm( const atomic_terms_t& bool_expr );

    /// @brief Evaluate the atomic term with a vector that gives the truth values of all trivial 
    ///     variables of the algebraic term
    /// @param args A bool dict with all variables set.
    /// @return The evaluated truth for the algebraic expression with the values of the parameter dict.
    bool eval( const BoolDict<BoolValue>& args ) const;

    /// @brief Evaluate the atomic term with a vector that gives the truth values of all trivial 
    ///     variables of the algebraic term
    /// @param args A bool dict with all variables set.
    /// @return The evaluated truth for the algebraic expression with the values of the parameter dict.
    template< typename B >
        requires is_bool_evaluatable< B >
    bool eval( const BoolDict<B>& args ) const {
        return this->eval( static_cast<BoolDict<BoolValue>>(args) );
    }
};

} // namespace truth

