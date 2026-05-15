// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <map>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>

#include "bool_value.hpp"


namespace truth {



/// @brief Check, if a type `C` is convertible to `T`.
/// @tparam T The type to check, if `C` is statically castable to this type
/// @tparam C The type to check, if it is statically castable to `T`
template< typename T, typename C >
concept is_convertible = requires () {
    typeid(static_cast<T>( std::add_lvalue_reference_t<C>() )).name() == typeid(T).name();// -> std::convertible_to<T>;
};

/// @brief Check, if a type `T` is evaluatable as `bool`
/// @tparam T The type, that is to be checked, if it is convertible to `bool`
template< typename T >
concept is_bool_evaluatable = requires () {
    is_convertible<bool, T>;
};


/// @brief A bool value dict. The truth values are ordered by a `const std::string`, for example for correspondence with
///  coherent univarite variable names
/// @tparam T The type of bool truth values to save in the dict for the key values. Defaults to `T = BoolValue`.
template< typename T = BoolValue >
    /*requires is_bool_evaluatable< T >*/
class BoolDict
    :   private std::map< std::string, T >
{
public:
    /// @brief Constructor
    BoolDict()
        :   std::map< std::string, T >()
    {}
    /// @brief Constructor that initializes the bool dict with a list of variable names
    /// @param ils A list of univariate variable names. All values are to be unique.
    BoolDict( std::initializer_list< std::string > ils )
        :   std::map< std::string, T >()
    {
        for ( const std::string& s : ils ) {
            if ( typeid(T).name() == typeid(BoolValue).name() )
                (*this)[s] = BoolValue::None;
            else (*this)[s] = false;
        }
    }
    
    /// @brief The type of the STL map from C++
    typedef typename std::map< std::string, T > self_t;

    using self_t::operator[];
    using self_t::at;
    using self_t::begin;
    using self_t::end;
    using self_t::size;

    using iterator = typename self_t::iterator;
    using const_iterator = typename self_t::const_iterator;

    /// @brief Get a reference to a bool type.
    /// @param key 
    /// @return A reference to bool, for better convenience the reference's value is also updated in the dictionary,
    ///  if modified.
    bool& getValue( const char* key ) const {
        try {
            T& value = this->at( key );
            return value;
        } catch ( std::out_of_range& oor ) {
            bool* bl = new bool(false);
            (*this)[key] = *bl;
            return *bl;
        }
        
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value The bool truth value to set the value to.
    void set( const char* key, bool& value ) {
        (*this)[key] = value;
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value The bool truth value to set the value to.
    void set( const char* key, BoolValue& value ) {
        (*this)[key] = value;
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value_func A bool evaluation function to set the value to behind evaluation
    void set( const char* key, bool (*value_func)() ) {
        (*this)[key] = value_func();
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value_func A bool evaluation function that thus accepts a (`*this`) `BoolDict` to set the value to
    ///  behind evaluation.
    void set( const char* key, bool (*value_func)(BoolDict& bd) ) {
        (*this)[key] = value_func(*this);
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value_func A bool evaluation function that thus accepts a (`bd`) `BoolDict` to set the value to
    ///  behind evaluation.
    /// @param bd A `BoolDict` to evaluate `value_func` with.
    void set( const char* key, bool (*value_func)(BoolDict& bdf), const BoolDict* bd ) {
        (*this)[key] = value_func(*bd);
    }

    /// @brief Set a value reference of a univariate key.
    /// @tparam B A somehow bool evaluatable and convertible-to `T` (the values in this dict) algebraic truth value
    /// @param key The key to set the value of.
    /// @param value The truth value to set the value to.
    /// @note As with the `BoolValue` class, it is ease-to-use, when assignment and conversion operators for `bool` and
    ///  `BoolValue` are part of `B`.
    template< typename B >
        requires is_bool_evaluatable< B > && is_convertible<B, T>
    void set( const char* key, B& value ) {
        T& t = static_cast<T>( value );
        (*this)[key] = t;
    }
};




} // namespace truth

