// Copyright 2026 The OnePointer Authors.
//

#pragma once

#include "ds_varenvironment.hpp"

#include <string_view>
#include <utility>


namespace oneptr {
namespace ds {


class ExprVarEnv
    :   protected std::string_view
    ,   public oneptr::ds::VarEnvironment
{
public:
    /// @brief The callback function of your library with which this class can get the value of a variable.
    typedef typename std::string (*getVarValue_f)(const std::string vname);

protected:
    const getVarValue_f getVarValue;

    void parse();

public:
    /// @brief Constructor. Parses in a logic like `varname:mapped_varname` the variables and adds it to environment
    /// @param var_append_list The `var_append_list`, e.g. of a regular expression.
    ExprVarEnv( const std::string var_append_list, const getVarValue_f gvv_func );

    /// @brief Get the values of all variables of this instance from the using library
    /// @return A pair with the values `{ found_values, all_found }`.
    std::pair< unsigned int, bool > getVarValues();

    const std::string str() const;
};


} // namespace ds
} // namespace oneptr
