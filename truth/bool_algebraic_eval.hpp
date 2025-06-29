// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "bool_algebraic_bind.hpp"
#include "bool_base.hpp"
#include "bool_value_dict.hpp"
#include "boolexpr.hpp"
#include "boolop.hpp"
#include "booltree.hpp"
#include "oneptr/queue.hpp"

namespace truth {

class EvaluationExam;

/// @brief The truth type, that also remembers the operator on the right side of the bool
typedef std::pair< bool, BoolOperator::Type > evaluated_truth_t;


/// @brief A group of algebraic (maybe disjunct) evaluation algebra
/// @note One might to inherit this and the `EvaluationExam` class for own and different use cases and prorefine with virtual functions.
class EvaluationGroup
    :   protected oneptr::queue< AlgebraicBind >
{
protected:

public:
    /// @brief The adjunct exam for this evaluation group
    EvaluationExam* exam;
    /// @brief The custom result of the evaluation of this classe's queue and it's `EvaluationExam`
    BoolValue result;

    using oneptr::queue< AlgebraicBind >::const_iterator;
    using oneptr::queue< AlgebraicBind >::front;
    using oneptr::queue< AlgebraicBind >::back;
    using oneptr::queue< AlgebraicBind >::pop_front;
    using oneptr::queue< AlgebraicBind >::begin;
    using oneptr::queue< AlgebraicBind >::end;
    using oneptr::queue< AlgebraicBind >::size;
    using oneptr::queue< AlgebraicBind >::insert;
    using oneptr::queue< AlgebraicBind >::insert_range;
    using oneptr::queue< AlgebraicBind >::push_back;
    using oneptr::queue< AlgebraicBind >::append_range;

    /// @brief Constructor
    /// @param logic_terms An initializer list with pointers to algebraic bind object, that concern complete,
    ///   similary operative with more than two elemental operands evaluatable objects. Have a look at `AlgebraicBind`
    ///     and `AlgebraicSmallTerm`
    EvaluationGroup( std::initializer_list< AlgebraicBind* > logic_terms );
    /// @brief Constructor
    /// @param expr The logic-algebraic bool expression to evaluate and have insights here
    EvaluationGroup( const BoolExpr& expr );
    /// @brief Constructor
    /// @param expr The logic-algebraic bool expression to evaluate and have insights here
    EvaluationGroup( const std::string expr );

    /// @brief Get the `EvaluationExam` of this class
    /// @return A pointer to an `EvaluationExam` for the set of logical expressions of this instance of the class
    virtual EvaluationExam* get_evaluation_exam();

    /// @brief Evaluate this complete classes set of logical expressions with an evaluation bool dict
    /// @tparam B The type of bool value. Defaults to library intern.
    /// @param bd The bool dict for evaluation of covariate variable names
    /// @return The truth for all algebraic bind objects in this queue.
    template< typename B = BoolValue >
        requires is_bool_evaluatable< B >
    bool eval_if_complete( const BoolDict< B >& bd ) const{

        bool result = true;
        unsigned int idx = 0;
        BoolOperator::Type previous_operator = BoolOperator::UNKNOWN;
        for ( oneptr::queue_element< AlgebraicBind >* qe_ab : *this ) {

            AlgebraicBind* ab = qe_ab->get();
            
            auto evalfunc = [bd](const BoolExpr& bexpr) {
                if ( bexpr.size() > 0 ) {
                    Tree* tree = Tree::get_new( &bexpr );
                    return (BoolValue) tree->eval( &bd );
                }
                return (BoolValue) BoolValue::None;
            };
            
            BoolValue val_left = evalfunc( ab->value.first.first );
            BoolValue val_right = evalfunc( ab->value.first.second );

            BoolValue res = BoolOperator::eval( val_left, ab->value.second, val_right );
            ab->truth.first = val_left;
            ab->truth.second = val_right;

            result = result && res;
            ++idx;
        }

        return result;
    }
};


class Evaluator;


/// @brief The evaluation exam to build and investigate on smaller and greater contexts of logic-arithmetic expressions.
class EvaluationExam {
public:
    /// @brief The reference to the corresponding group of algebraically ordered expressions
    EvaluationGroup const& evaluation_group;

    /// @return `this->evaluated`
    bool is_evaluated() const;

    /// @brief Evaluate the complete `this->evaluation_group` to `this->small_terms`
    virtual void evaluate_all_groups();
    /// @brief Evaluate the `this->small_terms` with a bool dict.
    /// @param bd The reference to a `BoolDict<BoolValue>`
    virtual void evaluate_exam( const BoolDict<BoolValue>& bd );
    /// @brief Call evaluation function inside the evaluation group and use its algorithm
    /// @param bd The reference to a `BoolDict<BoolValue>`
    /// @note Calls `this->evaluation_group.eval_if_complete( bd );` instead of `evaluate_exam();`.
    virtual void evaluate_group( const BoolDict<BoolValue>& bd );

protected:
    /// @brief Simple bool flag for testing purposes
    bool evaluated;
    /// @brief A queue of `AlgebraicSmallTerm`. Needs to be build and parsed with `evaluate_all_groups()`.
    oneptr::queue< AlgebraicSmallTerm > small_terms;
    /// @brief The result of all evaluation.
    BoolValue value;

    /// @brief Constructor. Copy constructs `eval_group`.
    /// @param eval_group The `EvaluationGroup` to construct this `EvaluationExam` instance for
    EvaluationExam( const EvaluationGroup& eval_group );
    /// @brief Constructor. References `eval_group`.
    /// @param eval_group The `EvaluationGroup` to construct this `EvaluationExam` instance for
    EvaluationExam( EvaluationGroup& eval_group );
    /// @brief Copy constructor
    /// @param evaluation_exam The `EvaluationExam` to copy.
    EvaluationExam( const EvaluationExam& evaluation_exam );

    friend class EvaluationGroup;
    friend class Evaluator;
};


/// @brief 
class Evaluator
{
protected:
    /// @brief All `EvaluationExam`s known to this `Evaluator`
    oneptr::queue< EvaluationExam > exams;
    /// @brief All `EvaluationGroups`s known to this `Evaluator`
    oneptr::queue< EvaluationGroup > groups;

public:
    /// @brief The `BoolType` of this evaluator
    BoolType& type;
    /// @brief The `BoolValue` of this evaluator
    BoolValue& value;
    /// @brief The `BoolExpr` of this evaluator
    BoolExpr expr;

    /// @brief Constructor
    Evaluator();
    /// @brief Constructor. Copy-constructs (by-reference) from `bool_type`
    /// @param bool_type The `BoolType` to reference and copy-construct
    Evaluator( BoolType& bool_type );

    /// @brief Append some logical-algebraic expression to the current `BoolExpr expr`
    /// @param algebraic_logic_expression The algebraic logical term to insert
    /// @return `*this`
    /// @note You may have your own version of this function in your inheriting classes 
    virtual Evaluator& operator<<( const std::string algebraic_logic_expression );
    /// @brief Set  some logical-algebraic expression to the current `BoolExpr expr`
    /// @param algebraic_logic_expression The algebraic logical term to insert
    virtual void set_expr( const std::string algebraic_logic_expression );
    /// @brief Clear the current `BoolExpr expr`.
    virtual void clear_expr();

    /// @brief Evaluate everything with a `BoolDict<BoolValue>` that holds correspondence values for unique variable names
    /// @param bd The bool dict to use for evaluation
    /// @return The result as default C++ `bool`
    /// @note You may have your own version of this function in your inheriting classes 
    virtual bool eval( const BoolDict< BoolValue >& bd );
    
    /// @brief Get a `BoolDict<BoolValue>` with all variables set to `BoolValue::None` until filled with semantics by the business client
    /// @return A reference to a bool dict with all variables coherently found in `this->groups` set to `None`.
    /// @note You may have your own version of this function in your inheriting classes  
    virtual BoolDict< BoolValue >& getDict() const;
    
    /// @brief Get all `EvaluationExam`s of this evaluator
    /// @return A `oneptr::queue< EvaluationExam >` with the evaluation exams.
    const oneptr::queue< EvaluationExam > get_exams() const;
    /// @brief Get all `EvaluationGroup`s of this evaluator
    /// @return A `oneptr::queue< EvaluationGroup >` with the evaluation groups.
    const oneptr::queue< EvaluationGroup > get_groups() const;
    /// @brief Clear `this->exams` and `this->groups`
    void clear();
};



} // namespace truth


