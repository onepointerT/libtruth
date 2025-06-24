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



namespace truth {




/// @brief The functionality a bool value of this library has, a simple `enum class` with transformation and assignment operators.
/// @note Can be initialized e.g. with `BoolValue val() = true;`. Is also fully compatible inside of vectors and can use `static_cast`.
class BoolValue {
public:
    /// @brief All values that a bool type can have within this library.
    enum Type : uint8_t {
        True = 1,
        False = 0,
        None = 3
    };

    /// @brief The conversion between the libtruth library and the general C++ bool functionality
    union Value {
        /// @brief The in-library Bool type
        BoolValue::Type t;
        /// @brief The generic C++ bool type
        bool b;

        /// @brief Update, if `b != t` the values with `*this = b`.
        void update() { if ( b != t ) *this = b; }

        /// @brief The value `True`
        static constexpr uint8_t True = 1;
        /// @brief The value `False`
        static constexpr uint8_t False = 0;
        /// @brief The value `None`, thus `undefined` also
        static constexpr uint8_t None = 3;
        
        /// @brief Conversion operator to C++ default type `bool`
        operator bool() const { return t == BoolValue::True; }
        /// @brief Conversion operator to the default numeric logic-algebraic type `uint8_t`
        operator uint8_t() const { return ( ( t == BoolValue::True ) ? Value::True : ( ( t == BoolValue::False ) ? Value::False : Value::None ) );  }
        /// @brief Conversion operator to the enum `BoolValue::Type` of the `enum class BoolValue`
        operator BoolValue::Type() const { return t; }

        /// @brief Assignment operator for logic-algebraic small enumeration type `uint8_t`
        /// @param value The value, e.g. from `BoolValue::Type`
        void operator=( const uint8_t value ) { t = ( value == Value::True ? BoolValue::True : ( value == Value::False ? BoolValue::False : BoolValue::None ) ); b = t;  }
        /// @brief Assignment operator for logic-algebraic C++-generic type `bool`
        /// @param value The value, e.g. `true`, `false` or a condition
        void operator=( const bool value ) { b = value; t = ( value ? BoolValue::True : BoolValue::False ); }
        /// @brief Assignment operator for logic-algebraic library-default type `BoolValue::Type`
        /// @param value The value, e.g. `True`, `False` or `None`
        void operator=( const BoolValue::Type value ) { b = ( value == BoolValue::True ? true : false ); t = value; }

        /// @brief The comparison operator for equivalence with `union BoolValue::Value`
        /// @param value The value to compare with
        /// @return True, if `value == *this`
        bool operator==( const BoolValue::Value value ) const { return value.t == this->t && value.b == this->b; }
        /// @brief The negation comparison operator for equivalence with `union BoolValue::Value`
        /// @param value The value to compare with
        /// @return True, if `value != *this`
        bool operator!=( const BoolValue::Value value ) const { return !(value == *this); }

        /// @brief The comparison operator for equivalence with `enum BoolValue::Type`
        /// @param value The value to compare with
        /// @return True, if `value == *this`
        bool operator==( const BoolValue::Type value ) const { return t == value && b == value; }
        /// @brief The negation comparison operator for equivalence with `enum BoolValue::Type`
        /// @param value The value to compare with
        /// @return True, if `value != *this`
        bool operator!=( const BoolValue::Type value ) const { return t != value && b != value; }
    };

    /// @brief The value of this `BoolValue`
    Value value;

    /// @brief Constexpression constructor. Default constructor.
    constexpr BoolValue() : value( Type::None ) {}
    /// @brief Constexpression constructor. 
    /// @param val A `BoolValue::Type` value to assign to `this->value`
    constexpr BoolValue( const BoolValue::Type val ) : value( val ) {}
    /// @brief Constexpression constructor. 
    /// @param val A `bool` value to assign to `this->value`
    constexpr BoolValue( const bool val ) : value( val ? True : False ) {}

    /// @brief Conversion operator to C++ default type `bool`
    operator bool() const { return value; }
    /// @brief Conversion operator to the default numeric logic-algebraic type `uint8_t`
    operator uint8_t() const { return value; }
    /// @brief Conversion operator to the enum `BoolValue::Type` of the `enum class BoolValue`
    operator Type() const { return value; }

    /// @brief Assignment operator for logic-algebraic library-default type `BoolValue::Type`
    /// @param value The value, e.g. `True`, `False` or `None`
    BoolValue& operator=( const BoolValue::Type value ) { this->value = value; return *this; }
    /// @brief Assignment operator for logic-algebraic C++-generic type `bool`
    /// @param value The value, e.g. `true`, `false` or a condition
    BoolValue& operator=( const bool value ) { this->value = value; return *this; }

    /// @brief The comparison operator for equivalence with `union BoolValue::Value`
    /// @param value The value to compare with
    /// @return True, if `this->value == value`
    bool operator==( const BoolValue::Value value ) const { return value == this->value; }
    /// @brief The comparison operator for equivalence with `enum BoolValue::Type`
    /// @param value The value to compare with
    /// @return True, if `this->value == value`
    bool operator==( const BoolValue::Type value ) const { return value == this->value; }

    


    /// @brief `AND` operator of this library
    /// @param lvalue The left value
    /// @param rvalue The right value
    /// @return `lvalue && rvalue`
    bool operator &&( const BoolValue rvalue ) { return this->value && rvalue; }
    /// @brief `OR` operator of this library
    /// @param lvalue The left value
    /// @param rvalue The right value
    /// @return `lvalue || rvalue`
    bool operator ||( const BoolValue rvalue ) { return this->value || rvalue; }
    /// @brief `IMPLIES` operator of this library
    /// @param lvalue The left value
    /// @param rvalue The right value
    /// @return `(lvalue || rvalue) && rvalue`
    bool operator >>( const BoolValue rvalue ) { return ( this->value || rvalue ) && (bool)rvalue; }
    /// @brief `NEGATION` operator of this library
    /// @param lvalue The value
    /// @return `! value`
    bool operator !() { return ! this->value; }
};

/// @brief Needed for the concepts `is_convertible` and `is_bool_evaluatable`, that need a value for testing the trueness of `static_cast`-functionalability.
/// @tparam T The generic template parameter from libstdc++, will evaluate to `BoolValue` here.
template< class T >
using add_lvalue_reference_t = typename std::add_lvalue_reference<BoolValue> (BoolValue());// { BoolValue() };

/// @brief Set and initialize `varname` to `BoolValue::False`
#define TRUTH_BOOL_FALSE(varname) BoolValue #varname = BoolValue::False;
/// @brief Set and initialize `varname` to `BoolValue::True`
#define TRUTH_BOOL_TRUE(varname) BoolValue #varname = BoolValue::True;
/// @brief Set and initialize `varname` to `BoolValue::None`
#define TRUTH_BOOL_NONE(varname) BoolValue #varname = BoolValue::None;


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
    requires is_bool_evaluatable< T >
class BoolDict
    :   private std::map< const std::string, T >
{
public:
    /// @brief Constructor
    BoolDict()
        :   std::map< const std::string, T >()
    {}
    /// @brief Constructor that initializes the bool dict with a list of variable names
    /// @param ils A list of univariate variable names. All values are to be unique.
    BoolDict( std::initializer_list< std::string > ils )
        :   std::map< const std::string, T >()
    {
        for ( const std::string& s : ils ) {
            if ( typeid(T).name() == typeid(BoolValue).name() )
                (*this)[s] = BoolValue::None;
            else (*this)[s] = false;
        }
    }
    
    /// @brief The type of the STL map from C++
    typedef typename std::map< const std::string, T > self_t;

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
    void set( const char* key, bool (*value_func)(BoolDict bd) ) {
        (*this)[key] = value_func(*this);
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value_func A bool evaluation function that thus accepts a (`bd`) `BoolDict` to set the value to
    ///  behind evaluation.
    /// @param bd A `BoolDict` to evaluate `value_func` with.
    void set( const char* key, bool (*value_func)(BoolDict bdf), const BoolDict* bd ) {
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

