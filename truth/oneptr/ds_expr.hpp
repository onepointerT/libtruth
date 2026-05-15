// Copyright (C) 2026 The OnePointer Authors.
//

#pragma once


#include "ds_expression_varenv.hpp"

#include <string_view>


namespace oneptr {
namespace ds {


class Expression
    :   protected std::string_view
    ,   public oneptr::ds::ExprVarEnv
{
public:
    Expression( const std::string expr_str, ExprVarEnv::getVarValue_f gvv_func );

    const std::string str_expr() const;
    std::string_view& expr() const;
};


} // namespace ds
} // namespace oneptr