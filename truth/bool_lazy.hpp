// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <memory>
#include <utility>

#include "bool_atomic_term.hpp"
#include "bool_value.hpp"

namespace truth {

/// @brief An `shared_ptr<AtomicBoolTerm>` for the further use in function
typedef std::shared_ptr< AtomicBoolTerm > atomic_bool_ptr;
/// @brief An `unique_ptr` of a complete `BoolDict`
typedef std::unique_ptr< BoolDict<> > bool_dict_ptr;
/// @brief A truth pair, consisting of an `atomic_bool_ptr` and a `bool_dict_ptr`.
typedef std::pair< atomic_bool_ptr, bool_dict_ptr > truth_pair_t;

/// @brief Get a new `truth_pair_t` made from a pointer of an `AtomicBoolTerm`.
/// @param abt The pointer to an `AtomicBoolTerm`
/// @return A `truth_pair_t` with all keys of the dict already set.
/// @note You may also call `truth::get_new( new AtomicBoolTerm(expr) );`
truth_pair_t& get_new( AtomicBoolTerm* abt );
/// @brief Evaluate a `truth_pair_t`
/// @param tp The truth pair with the values of the keys of the bool value dict already set.
/// @return The value of evaluation
BoolValue eval( const truth_pair_t& tp );

} // namespace truth
