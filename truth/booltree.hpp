// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <utility>

#include "boolexpr.hpp"
#include "bool_algebraic_bind.hpp"
#include "bool_atomic_term.hpp"
#include "bool_value_dict.hpp"
#include "oneptr/tree.hpp"

namespace truth {

// Forward declaration
class TreeLeaf;


/// @brief The content of a `Tree::node_t` tree node. Holds everything for investigating on evaluation results of a `BoolExpr`,
///   that is to be splitted into at least two operand parts per node and can be evaluated as `AlgebraicSmallTerm` per `TreeLeaf`.
/// @note Can also be build with factorizational splitting for all same-level operands with the same operator, in thus case, the alghorithm
///     is a little bit other than the default and needs to be implemented by the user. The default alghorithm will create two sub-nodes per
///     (TODO: not-yet `AlgebraicSmallTerm`) `AlgebraicBind` with two operands and one leaf for each small term's 1-2 operands, when the
///     `BoolExpr` of the `TreeNode` is small enough yet. Does honor brackets on investigating on the operands.
class TreeNode
{
    /// @brief The evaluation term member
    AlgebraicBind& m_term;
    /// @brief The bool expression of this `TreeNode`
    const BoolExpr& m_expr;
    /// @brief The evaluated bool value of this `TreeNode`.
    /// @note Defaults to `BoolValue::Unknown` before evaluation and `BoolValue::None` on evaluation errors.
    BoolValue& m_value;
public:
    /// @brief The operator that the factorizatored operand sub-nodes have in common
    BoolOperator::Type op;
    /// @brief The deepness of this node since `Tree::root`
    uint16_t deepness;

    /// @brief For evaluation concerns and better handability, the left neightbour, if any, of a term with a right operand
    /// @note On default, the 1st sub-node is the left term, the left operand of the operator
    TreeNode* neighbour_left;
    /// @brief For evaluation concerns and better handability, the left neightbour, if any, of a term with a right operand
    /// @note On default, the 2nd sub-node is the right term, the right operand of both
    TreeNode* neighbour_right;
    /// @brief The leaf, if any, with the `AlgebraicSmallTerm` of exactly this node's `BoolExpr m_expr`.
    TreeLeaf* leaf;

    /// @brief Constructor
    /// @param algebraic_term `AlgebraicBind` to evaluate and save for further investigation with this node.
    /// @param lneightbour The left neighbour's node-content, if any.
    /// @param rneightbour The right neighbour's node-content, if any.
    TreeNode( AlgebraicBind& algebraic_term, TreeNode* lneightbour = nullptr, TreeNode* rneightbour = nullptr );
    /// @brief Constructor
    /// @param algebraic_term The `BoolExpr` to evaluate and save for further investigation with this node.
    /// @param logic_operator The logical-arithmetic operator, that thus this node has, concatenating its neighbour to the term of `this->parent->content->expr()`.
    /// @param left_term If `algebraic_term` is the left logical-arithmetic operand.
    /// @param lneightbour The left neighbour's node-content, if any.
    /// @param rneightbour The right neighbour's node-content, if any.
    TreeNode( const BoolExpr& algebraic_term, BoolOperator::Type logic_operator, bool left_term = false, TreeNode* lneightbour = nullptr, TreeNode* rneightbour = nullptr );

    /// @brief Get the result value of the over-all investigation/evaluation of this node's attributes
    /// @return The result value of `this->leaf` or, concatenated with `this->op` and `BoolOperator::eval()`, the result of at least both sub-nodes and their recursive
    ///     result onto until the leafs.
    const BoolValue& value() const;
    /// @brief Get the underlying `BoolExpr` of this node in completness
    /// @return A reference to the logic-algebraic bool expression of this node
    const BoolExpr& expr() const;
    /// @brief Get the used `AlgebraicBind` term with all parts of the term of `expr()` and a further insight of its operand parts.
    /// @return A reference to the logic-algebraic `AlgebraicBind` object of this node
    const AlgebraicBind& term() const;
    /// @brief Get a reference to a pair of `BoolValue`, that is thus the result of the left and the right sub-node.
    /// @return A reference to an `AlgebraicBind::truth_t`, that defaults to `BoolValue::Unknown` until evaluated.
    AlgebraicBind::truth_t& terms_truth();

    operator bool() const;
    operator BoolValue() const;

    /// @brief A pair of node-content `TreeNode` pointers
    typedef std::pair< TreeNode*, TreeNode* > node_children_pair_t;

    /// @brief Split `this->expr()` at last operator or before the last most-upper backet into two sub-node content-objects
    /// @return A pair with two `TreeNode` pointers. If one of them is a `nullptr`, the term of this side of the operator is elementaristic
    ///     enought to create a `Tree::leaf_t`/`TreeLeaf`
    node_children_pair_t* split() const;
    /// @brief Create a new `leaf_t::content` for the complete `TreeNode::expr`. This can be senseful, if `TreeNode::split() == {nullptr, nullptr}`.
    /// @return A pointer to a `TreeLeaf` created from the complete `this->m_expr`.
    TreeLeaf* new_leaf() const;
};


/// @brief The content of the last and latestly smallest nodes of a tree path, so called `TreeLeaf`.
class TreeLeaf {
public:
    /// @brief The pointer to the convergently evaluatable `AlgebraicSmallTerm`, that is thus evaluatable by coherent univariate varibles and
    ///  a operator.
    AlgebraicSmallTerm* term;
    /// @brief The result of the evaluation of this `TreeLeaf`
    BoolValue& result;

    /// @brief Constructor
    /// @param ast The algbraic small term to investigate on with this last-and-no-subs node.
    TreeLeaf( AlgebraicSmallTerm* ast );
};



/// @brief The binary tree for sorting, searching, investigating and evaluating algebraic logical truth terms.
/// @note Inherits `oneptr::Tree< TreeNode, TreeLeaf >`.
class Tree
    :   protected oneptr::Tree< TreeNode, TreeLeaf >
{
public:
    /// @brief The `BoolExpr` that was given to the root of this tree.
    const BoolExpr* expr;

    typedef oneptr::Tree< TreeNode, TreeLeaf > self_t;

    /// @brief Get a new logical-inherent tree from an algebraic expression like `a && b || c >> a && (b || c)`
    /// @param expr The `BoolExpr` to evaluate with this Tree
    /// @return A pointer to a newly-created `Tree`
    static Tree* get_new( const BoolExpr* expr );
    
    /// @brief Build all leafs that are needed for `node`
    /// @param node The node to have an insight on
    void build_leafs_of_node( node_t* node );
    /// @brief Build the node by having every sub-nodes set up
    /// @param node The node to evaluate
    void build_node( node_t* node );
    /// @brief Build all nodes from `node` onwards with `logical_expression`
    /// @param logical_expr The logical expression to use for the root
    /// @param node The node to start with
    /// @return The current node on success, nullptr on failure
    node_t* build( const BoolExpr& logical_expr, node_t* node = nullptr );
    /// @brief Build the root node only
    /// @param logical_expr The logical expression that is to be evaluated by the root node and its recursive sub-nodes
    void build_root( const BoolExpr& logical_expr );

    /// @brief Constructor
    /// @param atomic_bt A pointer to a `AtomicBoolTerm`
    Tree( const AtomicBoolTerm* atomic_bt );
    /// @brief Constructor
    /// @param atomic_expr A pointer to a `BoolExpr`
    Tree( const BoolExpr* atomic_expr );

    void reset( const AtomicBoolTerm* abt );
    void reset( const AlgebraicBind* ab );

    using self_t::node_t;
    using self_t::leaf_t;
    using self_t::node_iterator;
    using self_t::leaf_iterator;
    using self_t::root;
    using self_t::nodes;
    using self_t::leafs;
    using self_t::depthTo;

    leaf_t* most_left_leaf;
    leaf_t* most_right_leaf;
    
    /// @brief Evaluate a `leaf_t` with a bool truth dictionary
    /// @param leaf The leaf to evaluate
    /// @param bd The bool dict.
    /// @return True, if `leaf->content->term` evaluates to true.
    BoolValue eval( const leaf_t* leaf, const BoolDict<BoolValue>* bd ) const;
    /// @brief Evaluate a `node_t` with a bool truth dictionary
    /// @param leaf The node to evaluate
    /// @param bd The bool dict.
    /// @return True, if `node->subs` and `node->leafs` evaluate to true, by using `node->content->op`.
    BoolValue eval( const node_t* node, const BoolDict<BoolValue>* bd ) const;
    /// @brief Evaluate the complete tree starting at `Tree::root`
    /// @param bd The bool dict.
    /// @return The truth value of the recursivly evaluated `Tree::root`.
    BoolValue eval( const BoolDict<BoolValue>* bd ) const;

    operator BoolValue() const;
    operator bool() const;
};


} // namespace truth