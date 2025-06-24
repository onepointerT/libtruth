// Copyright 2025 The OnePointer Authors.
//

#pragma once



#include <cstddef>
#include <deque>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "bool_value_dict.hpp"
#include "oneptr/queue.hpp"




/// @brief The namespace of libtruth
namespace truth {

/// @brief The simple state of term evaluation
enum BoolState { LEFT, OP1, MID, OP2, RIGHT };
/// @brief The simple operators enum that can be between atomic terms of operands in logic-algebraic terms
//enum Operator { AND, OR }; // TODO: IMPLIES
/// @brief The categories that logic-algebraic terms can have in written shape and formula.
enum BoolTerm { LBRACKET, OP, VARIABLE, RBRACKET, TERM, NEGATION, NOTERM, OPERATOR, BRACKETTERM };


/// @brief A basic bool expression like it can be found in truth algebra and logics.
class BoolExpr
    :   public std::string
{
public:
    /// @brief Constructor
    BoolExpr();
    /// @brief Constructor
    /// @param exprstr The expression string that is to be saved to this class
    BoolExpr( const char* exprstr );
    /// @brief Constructor
    /// @param exprstr The expression string that is to be saved to this class
    BoolExpr( const std::string exprstr );
    /// @brief Constructor
    /// @param exprstr The expression string that is to be saved to this class
    BoolExpr( const std::string_view exprstr );

    /// @brief Get to know the category of the logic term that this expression has.
    /// @return The category of this logic expression, or `BoolTerm::TERM`, if it is not distinguishable uniquely.
    BoolTerm is() const;

    /// @brief Assignment operator
    /// @param exprstr The expression to assign
    void operator=( const char* exprstr );
    /// @brief Comparison operator
    /// @param another The expression to compare
    /// @return True, if both expression are the same
    bool operator==( const std::string another );
    /// @brief Comparison operator
    /// @param another The expression to compare
    /// @return True, if both expression are not the same
    bool operator!=( const std::string another );
    /// @brief The expression to substract from this, e.g. evaluated expressions can be substracted from `BoolExpr`
    ///  to get the rest (unevaluated) expression.
    /// @param substract The expression to substract
    /// @return `(*this) - substract`.
    BoolExpr& operator-=( const std::string substract );
    /// @brief The expression to substract from this, e.g. evaluated expressions can be substracted from `BoolExpr`
    ///  to get the rest (unevaluated) expression.
    /// @param substract The expression to substract
    /// @return `(*this) - substract`.
    BoolExpr& operator-( const std::string substract );

    std::string find_prefix_before( const std::string expr );
    std::string find_suffix_behind( const std::string expr );
};

/// @brief The simplified expression type of a `BoolExpr`
typedef BoolExpr expr_t;
/// @brief The categorized bool expression.
typedef std::pair< expr_t, BoolTerm > atomic_term_t;
/// @brief A pair of a complete competive `BoolExpr` and a pointer-to a queue with all substantial `atomic_term_t` inside.
typedef std::pair< expr_t, oneptr::queue< atomic_term_t >* > atomic_terms_t;
/// @brief A queue with multiple `atomic_terms_t` inside.
typedef oneptr::queue< atomic_terms_t > atomic_expr_t;

} // namespace truth

/// @brief Iterate over the `BoolState` of the current evaluation period by incrementing the enum
/// @param state The current value of the evaluation status
/// @return The new value of the evaluation status
constexpr truth::BoolState operator++( const truth::BoolState state );

