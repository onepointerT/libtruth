// Copyright 2025 The OnePointer Authors.
//

#include <any>
#include <functional>
#include <type_traits>

#include "bool_base.hpp"
#include "bool_value.hpp"



namespace truth {


bool BoolValue::is_parsable( const char* value ) {
    std::string vstr = value;
    return vstr == "true" || vstr == "True" || vstr == "false" || vstr == "False"
            || vstr == "none" || vstr == "None" || vstr == "unknown" || vstr == "Unknown";
}


BoolValue::Type BoolValue::parse( const char* value ) {
    std::string vstr = value;
    if ( vstr == "true" || vstr == "True" ) return True;
    else if ( vstr == "false" || vstr == "False" ) return False;
    else if ( vstr == "none" || vstr == "None" ) return None;
    else return Unknown;
}


} // namespace truth

