// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include "boolexpr.hpp"
#include "bool_atomic_term.hpp"
#include "bool_value_dict.hpp"
#include "oneptr/tree.hpp"

namespace truth {


class TreeNode {
public:
    const BoolOperator op;
    const BoolExpr* expr_left;
    const BoolExpr* expr_right;
};


class TreeLeaf {
public:
    BoolValue result;
};



class BoolTree
    :   oneptr::Tree< TreeNode, TreeLeaf >
{
public:
    BoolTree( const AtomicBoolTerm* atomic_bt );
    BoolTree( const BoolExpr* atomic_expr );

    BoolValue eval( const BoolDict* bd );
    BoolValue eval( const std::vector<BoolValue> bd );

    operator BoolValue() const;
    operator bool() const;
}


} // namespace truth