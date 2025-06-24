// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <memory>
#include <utility>

#include "bool_atomic_term.hpp"
#include "bool_value_dict.hpp"

namespace truth {

/// @brief An `shared_ptr<AtomicBoolTerm>` for the further use in function
typedef std::shared_ptr< AtomicBoolTerm > atomic_bool_ptr;
/// @brief An `unique_ptr` of a complete `BoolDict`
typedef std::unique_ptr< BoolDict<> > bool_dict_ptr;
/// @brief A truth pair, consisting of an `atomic_bool_ptr` and a `bool_dict_ptr`.
typedef std::pair< atomic_bool_ptr, bool_dict_ptr > truth_pair_t;


truth_pair_t& get_new( const AtomicBoolTerm* abt );
BoolValue eval( const truth_pair_t& tp );

} // namespace truth
