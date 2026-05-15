// Copyright (C) 2026 The OnePointer Authors.
//

#pragma once


#include "ds_expr.hpp"


namespace oneptr {
namespace ds {


Expression::Expression( const std::string expr_str, ExprVarEnv::getVarValue_f gvv_func )
    :   std::string_view()
    ,   ExprVarEnv( "", gvv_func )
{
    const size_t pos_var_delimiter = expr_str.find_first_of( " | " );
    if ( pos_var_delimiter != expr_str.npos ) {
        const std::string expr = expr_str.substr( 0, pos_var_delimiter - 1 );
        const std::string varenv = expr_str.substr( pos_var_delimiter + 3 );
        ExprVarEnv::basic_string_view::swap( *new std::string_view(varenv) );
        ExprVarEnv::parse();
        std::string_view::swap( *new std::string_view(expr) );
    }
}

const std::string Expression::str_expr() const {
    return std::string_view::data();
}

} // namespace ds
} // namespace oneptr