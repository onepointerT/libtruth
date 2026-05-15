// Copyright 2026 The OnePointer Authors.
//

#pragma once

#include "ds_expression_varenv.hpp"

namespace oneptr {
namespace ds {


void ExprVarEnv::parse() {
    if ( std::string_view::length() == 0 ) return;
    else VarEnvironment::clear();
    size_t pos_next_space = 0;
    while ( pos_next_space != this->npos ) {
        const size_t pos_prev_space = pos_next_space;
        pos_next_space = this->find_first_of( ' ', pos_next_space );
        std::string varstr = this->substr( pos_next_space - 1, pos_next_space - pos_prev_space ).data();
        if ( varstr[varstr.length()-1] == ',' ) varstr.erase(varstr.length()-1);
        const size_t pos_double_point = varstr.find_first_of( ':' );
        if ( pos_double_point != varstr.npos ) {
            const std::string vname_original = varstr.substr( 0, pos_double_point - 1 );
            const std::string vname = varstr.substr( pos_double_point + 1 );
            this->add( vname, vname_original );
        }
    }
}


ExprVarEnv::ExprVarEnv( const std::string var_append_list, const getVarValue_f gvv_func  )
    :   std::string_view( var_append_list )
    ,   VarEnvironment()
    ,   getVarValue( gvv_func )
{
    this->parse();
}

std::pair< unsigned int, bool > ExprVarEnv::getVarValues() {
    unsigned int vv = 0;

    for ( typename VarEnvironment::iterator it = VarEnvironment::begin(); it != VarEnvironment::end(); ++it ) {
        const std::string vvalue = this->getVarValue(it->second->original_name);
        if ( vvalue.length() > 0 ) {
            ++vv;
            this->set( it->first, vvalue );
        }
    }

    return { vv, vv == VarEnvironment::size() };
}


const std::string str() const;

} // namespace ds
} // namespace oneptr
