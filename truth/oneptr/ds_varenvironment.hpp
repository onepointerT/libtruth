// Copyright 2026 The OnePointer Authors.
//

#pragma once

#include <map>
#include <string>
#include <string_view>

namespace oneptr {
namespace ds { // DataScience


class Variable final
    :   public std::string_view
{
public:
    const std::string name;
    const std::string original_name;

    Variable( const std::string vname, const std::string vvalue = "", const std::string orig_name = "" );

    Variable& setValue( const std::string vvalue = "" );
};


class VarEnvironment
    :   protected std::map< std::string, Variable* >
{
public:
    VarEnvironment();

    bool add( const std::string vname, const std::string vvalue = "" );
    Variable& set( const std::string vname, const std::string vvalue );
    Variable& get( const std::string vname );
    Variable& get( const std::string vname ) const;
    bool remove( const std::string vname );
};


} // namespace ds
} // namespace oneptr
