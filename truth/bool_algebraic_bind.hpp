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

// Forward declaration
class AlgebraicSmallTerm;


/// @brief 
class AlgebraicBind {
protected:
    oneptr::queue< AlgebraicSmallTerm > evaluated_terms;

public:
    /// @brief The basic type with a left and a right expression that can be matched with operators.
    typedef std::pair< BoolExpr&, BoolExpr& > expr_t;
    /// @brief The basic type that matches and `expr_t` with a `BoolOperator`.
    typedef std::pair< const expr_t, const BoolOperator::Type > bindval_t;
    /// @brief 
    typedef std::pair< BoolValue&, BoolValue& > truth_t;

    /// @brief The left and right expression of an operator, thus the operands. May be simple expressions or greater ones.
    bindval_t& value;
    /// @brief The over-all truth value of the expressions, without evaluating them each.
    truth_t truth;

    /// @brief Constructor
    /// @param expr_left The left operand expression
    /// @param expr_right The right operand expression
    /// @param op The logic algebra-operator
    AlgebraicBind( const std::string expr_left, const std::string expr_right, const BoolOperator::Type op );
    /// @brief Constructor
    /// @param expr_left The left operand expression
    /// @param expr_right The right operand expression
    /// @param op The logic algebra-operator
    AlgebraicBind( BoolExpr& expr_left, BoolExpr& expr_right, const BoolOperator::Type op );
    /// @brief Copy constructor
    /// @param bindval The `bindval_t` to copy
    AlgebraicBind( bindval_t& bindval );

    /// @brief Evaluate the `truth_t` value of `this->truth` with the operator
    /// @return `BoolOperator::eval( truth.first, value.second, truth.second )`
    bool eval() const;
    /// @brief Get the expression of this algebraic bind
    BoolExpr& string() const;

    /// @brief Get all small terms of found in `expr`
    /// @param expr The algebraic expression to search for smaller operands with their logic bind-operator.
    /// @return All pointers to `AlgebraicSmallTerm` found for value.first, at least one of them is a valid assertion.
    static const std::vector< AlgebraicSmallTerm* >& small_terms( const BoolExpr& expr );
    /// @brief Get all small terms of found in `value.first`
    /// @return All pointers to `AlgebraicSmallTerm` found for value.first, at least one of them is a valid assertion.
    /// @note The algebraic expression is searched for smaller operands with their logic bind-operator.
    const std::vector< AlgebraicSmallTerm* >& small_terms_left() const;
    /// @brief Get all small terms of found in `value.first`
    /// @return All pointers to `AlgebraicSmallTerm` found for value.first, at least one of them is a valid assertion.
    /// @note The algebraic expression is searched for smaller operands with their logic bind-operator.
    const std::vector< AlgebraicSmallTerm* >& small_terms_right() const;

    /// @brief Fill the dict `bd` with all variables of expr.
    /// @param expr The expression to search contained variables of
    /// @param bd The dict to fill with the new variables
    /// @return A reference to the bool dict `bd`
    static BoolDict<BoolValue>& fillDict( const bindval_t expr, BoolDict<BoolValue>& bd );

    void push_evaluated_small_term( AlgebraicSmallTerm* ast );

    static bool eval_expression_if_possible( const BoolExpr& expr, const oneptr::queue< AlgebraicSmallTerm > terms
                                           , BoolValue& result );
    bool eval_if_possible();
};


/// @brief 
class AlgebraicSmallTerm {
public:
    /// @brief A pair of varnames, at least one is set for algebraic small terms
    typedef std::pair< const std::string, const std::string > varnames_t;
    /// @brief 
    typedef std::pair< const varnames_t, const BoolOperator::Type > small_term_t;

    /// @brief The small term, containing one or two variable names and a logical operator
    small_term_t term;
    /// @brief A reference to the maybe bigger bool expression
    const BoolExpr& expr;
    /// @brief The result of the evaluation
    BoolValue value;
    /// @brief 
    AlgebraicBind* from;

    /// @brief Evaluate the small term to a bool value.
    /// @tparam B The bool type, defaults to `BoolValue`
    /// @param bd The value dict to lookup variable names in
    /// @return The truth value, that thus is also set in `this->value`
    template< typename B = BoolValue >
        requires is_bool_evaluatable< B >
    bool eval( const BoolDict<B>& bd, const B other = false ) {
        try {
            bool v1, v2 = false;
            if ( term.first.first.length() > 0 ) v1 = bd.at( term.first.first );
            else v1 = other;
            if ( term.first.second.length() > 0 ) v2 = bd.at( term.first.second );
            else v2 = other;
            
            value = BoolOperator::eval( v1, term.second, v2 );
            return value;
        } catch ( std::out_of_range& oor ) {
            return false;
        }
    }

    /// @brief Constructor
    /// @param var1 The name of the first variable
    /// @param var2 The name of the second variable
    /// @param op The operator between or in front of the variable(s)
    /// @param bexpr A reference to the maybe bigger `BoolExpr`
    AlgebraicSmallTerm( const std::string var1, const std::string var2, const BoolOperator::Type op, const BoolExpr& bexpr );
    /// @brief Constructor
    /// @param var1 The name of the first variable
    /// @param var2 The name of the second variable
    /// @param op The operator between or in front of the variable(s)
    /// @param bexpr A reference to the maybe bigger `BoolExpr`
    AlgebraicSmallTerm( const std::string var1, const std::string var2, const BoolOperator::Type op, AlgebraicBind* from_expr );
    /// @brief Copy Constructor
    /// @param stt A reference to the `small_term_t` to copy its values from
    AlgebraicSmallTerm( const small_term_t& stt );
    
    const std::string string() const { return term.first.first + BoolOperator::string(term.second) + term.first.second; }
    /// @brief Make a `AlgebraicSmallTerm` out of a `BoolExpr`
    /// @param algebraic_logic_expr A reference to the newly created `AlgebraicSmallTerm`
    static AlgebraicSmallTerm& new_small_term( const BoolExpr& algebraic_logic_expr );

    /// @brief Fill the dict `bd` with all variables of varnames.
    /// @param varnames The varnames to add, if not contained in `bd`
    /// @param bd The dict to fill with the new variables
    /// @return A reference to the bool dict `bd`
    static BoolDict<BoolValue>& fillDict( const varnames_t varnames, BoolDict<BoolValue>& bd );
    /// @brief Fill the dict `bd` with all variables of varnames in this class.
    /// @param bd The dict to fill with the new variables
    /// @return A reference to the bool dict `bd`
    BoolDict< BoolValue >& addToDict( BoolDict< BoolValue >& bd );
};

} // namespace truth


