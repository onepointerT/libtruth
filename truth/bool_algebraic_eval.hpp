// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "bool_algebraic_bind.hpp"
#include "bool_value_dict.hpp"
#include "boolop.hpp"
#include "oneptr/queue.hpp"

namespace truth {


typedef std::pair< bool, BoolOperator::Type > evaluated_truth_t;


class EvaluationGroup
    :   protected oneptr::queue< AlgebraicBind >
{
protected:

public:
    std::vector< evaluated_truth_t* > evaluated_truth;

    using oneptr::queue< AlgebraicBind >::const_iterator;
    using oneptr::queue< AlgebraicBind >::front;
    using oneptr::queue< AlgebraicBind >::back;
    using oneptr::queue< AlgebraicBind >::pop_front;
    using oneptr::queue< AlgebraicBind >::cbegin;
    using oneptr::queue< AlgebraicBind >::cend;
    using oneptr::queue< AlgebraicBind >::size;
};

class EvaluationExam {
public:
    typedef std::pair< AlgebraicSmallTerm*, evaluated_truth_t* > small_term_t;

protected:
    oneptr::queue< small_term_t > small_terms;
    const oneptr::queue_element< AlgebraicBind >* algebraic_surroundment;
};


class BoolEvaluator final {
protected:
    oneptr::queue< EvaluationExam > exams;
    oneptr::queue< EvaluationGroup > groups;
};



} // namespace truth


