// Copyright 2025 The OnePointer Authors.
//

#pragma once



#include <cstddef>
#include <deque>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "boolexpr.hpp"
#include "boolop.hpp"
#include "bool_algebraic_bind.hpp"
#include "bool_value.hpp"
#include "oneptr/queue.hpp"



namespace truth {


class AtomicBoolTerm;


/// @brief The trivial and enhanced bool type of this library
class BoolType
    :   public BoolValue
{
public:
    /// @brief The elemental value with a little enhancement over `bool` from C++
    typedef BoolValue Value;
    /// @brief The `BoolDict<BoolValue>` type that orders a truth value by its variable name
    typedef BoolDict<BoolValue> bool_dict_t;

    /// @brief The simple value of `True`
    inline static Value TRUE = BoolValue::True;
    /// @brief The simple value of `False`
    inline static Value FALSE = BoolValue::False;
    /// @brief The simple value of `None` or no value
    inline static Value NONE = BoolValue::None;
    /// @brief The simple value of `Undefined` or value not yet known
    inline static Value UNDEFINED = BoolValue::None;

    /// @brief The simple type of evaluated algebraic terms that can be inherent to their index
    ///  in a queue, a tree or appearence inside an algebraic atomic bool term.
    typedef typename std::map< const BoolExpr, BoolType::Value > arguments_t;
    /// @brief The `value_type` of `arguments_t` when iterating over it
    typedef typename std::pair< const BoolExpr, BoolType::Value > arguments_iter_t;

    /// @brief The current result of the evaluation
    arguments_t results;
    /// @brief The current truth values for all variables that need to be evaluated
    BoolDict< BoolValue >& truth_values;

    /// @brief Constructor
    /// @param args An initial result map with all yet evaluated results ordered by their `BoolExpr`.
    /// @param tvalues A `BoolDict` with all variable truth values ordered by their names.
    BoolType( const arguments_t args, bool value = false, BoolDict< BoolValue >* tvalues = nullptr );
    /// @brief Constructor
    /// @param value Initialize the value with this algebraic truth value.
    BoolType( const bool value = false );
    /// @brief Constructor
    /// @param value Initialize the value with this `BoolValue`.
    BoolType( const BoolType::Value value );

    /// @brief Get to know the `bool` value of `val`, a `BoolValue` 
    /// @param val The `BoolValue` to evaluate
    /// @return `true`, if `val == TRUE`, in each other case of `BoolValue` the value `false` is returned.
    static bool boolval( const BoolType::Value val );
    /// @brief Get to know, how all resulting truth arguments react, if you use only "&&" or "||" as operator
    ///   for all of them
    /// @param args A map of truth values ordered by their `BoolExpr`.
    /// @param op The operator to evaluate with
    /// @return Returns true, if forall `arg : args` is true for algebraic `BoolOperator` `op`.
    static bool boolval( const arguments_t args, const BoolOperator::Type op );

    /// @brief The basic type that a term has. It consists of a queue of elemental terms, the complete `BoolExpr`
    ///  and the rest of the categorization of the term
    typedef std::pair< atomic_terms_t*, BoolTerm > term_t;
    /// @brief The basic type that a expression has. It consists of a queue of elemental terms and the complete `BoolExpr`
    typedef std::pair< atomic_expr_t*, expr_t > expression_t;
    /// @brief The basic type that consists of a `BoolExpr` and maybe the rest and omitted part of the expression
    typedef std::pair< expr_t, expr_t > bool_expression_t;
    /// @brief The basic type that an atomic bool term has. It consists of a `AtomicBoolTerm` and the rest of the expression
    typedef std::pair< AtomicBoolTerm*, expr_t > atomic_bool_t;
    
    /// @brief Make a `BoolExpr` to a `AtomicBoolTerm`.
    /// @param expr The expression to evaluate
    /// @return The reference to a `atomic_bool_t`.
    static atomic_bool_t& expr( expr_t expr );

    /// @brief Find all terms in a `BoolExpr`. 
    /// @param expr The expression to evaluate
    /// @param restexpr A reference to a `bool_expression_t`, where all unevaluated expression left and right side of the
    ///  top-level terms is saved.
    /// @return A `term_t` with all top-level terms inside of the queue and the category set.
    /// @note For some reasons, if the terms found are in brackets or similar things, more basic terms
    ///  are not added to the queue and would need another call to this function
    static term_t find_terms( expr_t expr, bool_expression_t& restexpr );
    /// @brief Find all expressions inside a `BoolExpr`.
    /// @param expr The expression to evaluate
    /// @param restexpr A reference to a `bool_expression_t`, where all unevaluated expression left and right side of the
    ///  top-level terms is saved.
    /// @return A `expression_t` with all expressions in side of the queue and the complete evaluated expression set
    /// @note For some reasons, if the terms found are in brackets or similar things, more basic terms
    ///  are not added to the queue and would need another call to this function
    static expression_t find_exprs( expr_t expr, bool_expression_t& restexpr );

    /// @brief The type of an evaluated `bool_expression_t`.
    typedef std::pair< bool_expression_t, BoolTerm > bool_expr_t;
    /// @brief A vector of `bool_expr_t`
    typedef std::vector< bool_expr_t > expr_vec_t;

    /// @brief Find all atomic and substantial algebraic bool expressions
    /// @param expr The `BoolExpr` to evaluate
    /// @param max_terms The maximum number of terms to search for. 0 means "all of them".
    /// @return An `expr_vec_t` containing all terms found
    static expr_vec_t atomic_find( expr_t expr, size_t max_terms = 3 );
    /// @brief Find all atomic algebraic bool expressions in bracket pairs
    /// @param expr The `BoolExpr` to evaluate
    /// @param max_bracket_pairs The maximum number of bracket terms to search for. 0 means "all of them".
    /// @return An `expression_t` that holds one `atomic_expr_t` per bracket pair.
    static expression_t atomic_bracket_find( expr_t expr, size_t max_bracket_pairs = 1 );
    /// @brief Find all unevaluated expression part
    /// @param expr The complete `BoolExpr` to search in
    /// @param expr_match The `BoolExpr` already matched
    /// @return Returns a `bool_expression_t` with all unevaluated/unmatched rest expression to the left and the right of `expr_match`.
    static bool_expression_t atomic_restexpr_find( expr_t expr, expr_t expr_match );
    /// @brief Find out, how many atomic algebraic expressions are inside of a `BoolExpr`.
    /// @param expr The `BoolExpr` to count
    /// @return The number of atomic algebraic expressions inside of `expr`.
    static size_t atomic_expr_count( expr_t expr );

    /// @brief The basic type of all logic operators
    typedef BoolOperator::Type Operator;
    /// @brief The basic type of an algebraic term bound to an logic operator
    typedef AlgebraicBind::bindval_t bindval_t;
    /// @brief A basic queue containing `bindval_t`
    typedef oneptr::queue< bindval_t > bindval_queue_t;
    /// @brief A small term containing two variable names and an operator
    typedef AlgebraicSmallTerm::small_term_t small_term_t;
    /// @brief A basic queue of small terms
    typedef oneptr::queue< small_term_t > small_term_queue_t;

    /// @brief Find all matching and fitting small terms in an logic expression
    /// @param expr The `BoolExpr` to search in
    /// @return A pointer to a queue containing `AlgebraicSmallTerm::small_term_t` with all operators and their
    ///   bindshipment variables
    static small_term_queue_t* find_operator_in_expr( expr_t expr );
    /// @brief Find the first expression inside a more complete or more great expression
    /// @param expr The `BoolExpr` to search in
    /// @param restexpr A reference to a `BoolExpr` to give back the unevaluated context of the expression
    /// @return A pointer to a `AlgebraicBind::bindval_t` with all variables set.
    static bindval_t* find_first_expr_bind( expr_t expr, expr_t& restexpr );
    /// @brief Find all operators inside of a expression
    /// @param expr The `BoolExpr` to search in
    /// @param bindvals A reference to a `bindval_queue_t` that contains operands in the `bindval_t`, even without operator
    ///   yet set.
    /// @return A pointer to a queue containing `bindval_t`, that thus means operators bound to their two operands.
    static bindval_queue_t* find_operators( expr_t expr, bindval_queue_t& bindvals );
    /// @brief Find all operands in a `BoolExpr` and put them in a `bindval_queue_t`.
    /// @param expr The expression to evaluate
    /// @param restexpr A reference to a `expr_t`, where all unevaluated expression is put in
    /// @return A pointer to a `bindval_queue_t` with all operands already set.
    static bindval_queue_t* find_bindvals( expr_t expr, expr_t& restexpr );
    
    /// @brief Evaluate a bool expression with the help of a bool dict that can coherently give truth values by their name
    /// @param expr The expression to evaluate and search in
    /// @param res The resolution of all coherent truth values ordered by their variable name
    /// @return The truth value of `expr` according to `res`.
    static BoolType::Value evaluate_bool( expr_t expr, const BoolDict<BoolValue> res );
    /// @brief Evaluate a logic atomic bool expression
    /// @param expr The `atomic_expr_t == oneptr::queue<atomic_terms_t>` to evaluate all of
    /// @param res The bool type to save the answers of truth to and to find the truth values for coherent variables in.
    static void evaluate_bool_atomic( const atomic_expr_t expr, BoolType& res );
    /// @brief Evaluatie a logic algebraic bool term
    /// @param term The algebraic term to evaluate
    /// @param res The bool type to find the truth values for all variables in
    /// @return The value of the evaluation
    static BoolType::Value evaluate_bool_term( const atomic_terms_t term, const BoolType& res );
    /// @brief Evaluate a logic algebraic bool term by setting in the truth value of coherent variables from a `BoolDict`
    /// @param expr The atomic term to evaluate
    /// @param args The bool dict with all corresponding variables
    /// @return The truth value of the term after all of the variable values are substituted
    /// @note The truth values `BoolValue::None` and `BoolValue::Undefined` evaluate to `false`.
    static BoolType::Value eval_bool( const atomic_terms_t* expr, const BoolDict<BoolValue> args );
};


#define True BoolType::True
#define False BoolType::False
#define truth_true True
#define truth_false False
#define truth_true_str "true"
#define truth_false_str "false"


} // namespace truth

