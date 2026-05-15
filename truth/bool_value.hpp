// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <cstdint>
#include <type_traits>



namespace truth {




/// @brief The functionality a bool value of this library has, a simple `enum class` with transformation and assignment operators.
/// @note Can be initialized e.g. with `BoolValue val() = true;`. Is also fully compatible inside of vectors and can use `static_cast`.
class BoolValue {
public:
    /// @brief All values that a bool type can have within this library.
    enum Type : uint8_t {
        True = 1,
        False = 0,
        Unknown = 2,
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
        /// @brief The value `None`
        static constexpr uint8_t None = 3;
        /// @brief The value `Unknown`
        static constexpr uint8_t Unknown = 2;
        
        /// @brief Conversion operator to C++ default type `bool`
        operator bool() const { return t == BoolValue::True; }
        /// @brief Conversion operator to the default numeric logic-algebraic type `uint8_t`
        operator uint8_t() const { return ( ( t == BoolValue::True ) ? Value::True : ( ( t == BoolValue::False ) ? Value::False : ( t == BoolValue::None ? Value::None : Value::Unknown ) ) );  }
        /// @brief Conversion operator to the enum `BoolValue::Type` of the `enum class BoolValue`
        operator BoolValue::Type() const { return t; }

        /// @brief Assignment operator for logic-algebraic small enumeration type `uint8_t`
        /// @param value The value, e.g. from `BoolValue::Type`
        void operator=( const uint8_t value ) { t = ( value == Value::True ? BoolValue::True : ( value == Value::False ? BoolValue::False : ( value == Value::None ? BoolValue::None : BoolValue::Unknown )) ); b = t;  }
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
    bool operator >>( const BoolValue rvalue ) { return !this->value || rvalue; }
    /// @brief `NEGATION` operator of this library
    /// @param lvalue The value
    /// @return `! value`
    bool operator !() { return ! this->value; }

    static bool is_parsable( const char* value );
    static BoolValue::Type parse( const char* value );
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
/// @brief Set and initialize `varname` to `BoolValue::None`
#define TRUTH_BOOL_UNKNOWN(varname) BoolValue #varname = BoolValue::Unknown;



} // namespace truth

