// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>


namespace truth {


class NumericVariable {
public:
    union Value
    {
        std::string vname;
        double v;

        Value() :   vname("") {}

        Value& operator=( const std::string str ) { Value* val = new Value(); val->vname = str; return *val; }
        Value& operator=( const double dbl ) { Value* val = new Value(); val->v = dbl; return *val; }
        Value& operator=( const int i ) { Value* val = new Value(); val->v = static_cast<double>(i); return *val; }

        operator std::string() const { return vname; }
        operator double() const { return v; }
        operator int() const { return static_cast<int>(v); }
        operator unsigned int() const { return static_cast<int>(v) * (-1); }
    };
    
    Value value;

    NumericVariable( const std::string str )    :   value( str ) {}
    NumericVariable( const double dbl ) :   value( dbl ) {}
    NumericVariable( const int i ) :   value( i ) {}

    NumericVariable& operator=( const std::string str ) { NumericVariable* nv = new NumericVariable( str ); return *nv; }
    NumericVariable& operator=( const double dbl ) { NumericVariable* nv = new NumericVariable( dbl ); return *nv; }
    NumericVariable& operator=( const int i ) { NumericVariable* nv = new NumericVariable( i ); return *nv; }

    operator std::string() const { return value; }
    operator double() const { return value; }
    operator int() const { return value; }
    operator unsigned int() const { return value; }
};



/// @brief A numeric value dict. The truth values are ordered by a `const std::string`, for example for correspondence with
///  coherent univarite variable names
/// @tparam T The type of bool truth values to save in the dict for the key values. Defaults to `T = BoolValue`.
template< typename T = NumericVariable >
class NumericDict
    :   private std::map< const std::string, T >
{
public:
    /// @brief Constructor
    NumericDict()
        :   std::map< const std::string, T >()
    {}
    /// @brief Constructor that initializes the numeric dict with a list of variable names
    /// @param ils A list of univariate variable names. All values are to be unique.
    NumericDict( std::initializer_list< std::string > ils )
        :   std::map< const std::string, T >()
    {
        for ( const std::string& s : ils ) {
            (*this)[s] = *new double(0.0);
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

    /// @brief Get a reference to a numeric type.
    /// @param key 
    /// @return A reference to numeric, for better convenience the reference's value is also updated in the dictionary,
    ///  if modified.
    NumericVariable& getValue( const char* key ) const {
        try {
            T& value = this->at( key );
            return value;
        } catch ( std::out_of_range& oor ) {
            NumericVariable* nv = new NumericVariable(0.0);
            (*this)[key] = *nv;
            return *nv;
        }
        
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value The numeric truth value to set the value to.
    void set( const char* key, double& value ) {
        (*this)[key] = value;
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value The numeric truth value to set the value to.
    void set( const char* key, NumericVariable& value ) {
        (*this)[key] = value;
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value_func A numeric evaluation function to set the value to behind evaluation
    void set( const char* key, double (*value_func)() ) {
        (*this)[key] = value_func();
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value_func A numeric evaluation function that thus accepts a (`*this`) `NumericDict` to set the value to
    ///  behind evaluation.
    void set( const char* key, double (*value_func)(NumericDict& bd) ) {
        (*this)[key] = value_func(*this);
    }

    /// @brief Set a value reference of a univariate key.
    /// @param key The key to set the value of.
    /// @param value_func A numeric evaluation function that thus accepts a (`bd`) `NumericDict` to set the value to
    ///  behind evaluation.
    /// @param bd A `NumericDict` to evaluate `value_func` with.
    void set( const char* key, double (*value_func)(NumericDict& bdf), const NumericDict* bd ) {
        (*this)[key] = value_func(*bd);
    }

    /// @brief Set a value reference of a univariate key.
    /// @tparam N A somehow numeric evaluatable and convertible-to `T` (the values in this dict) algebraic truth value
    /// @param key The key to set the value of.
    /// @param value The truth value to set the value to.
    /// @note As with the `BoolValue` class, it is ease-to-use, when assignment and conversion operators for `bool` and
    ///  `BoolValue` are part of `N`.
    template< typename N >
    void set( const char* key, N& value ) {
        T& t = static_cast<T>( value );
        (*this)[key] = t;
    }
};



/// @brief The numerical comparison operators that are supported by the library
class NumericOperator {
public:
    enum Type : uint8_t {
        EQUAL = 0,
        GREATER = 1,
        GREATER_EQUAL = 2,
        SMALLER = 3,
        SMALLER_EQUAL = 4,
        UNEQUAL = 5,
        UNKNOWN = 8
    };

    static NumericOperator::Type is( const std::string str );
    static const std::string string( const NumericOperator::Type opt );

    static size_t find_next_of( const std::string str, const size_t pos = 0, const size_t _count = std::string::npos );

    static bool eval( const double lhs, const NumericOperator::Type nop, const double rhs );
    static bool eval( const std::string lhs, const NumericOperator::Type nop, const std::string rhs, const NumericDict<>& nd );
    static bool eval( const std::string lhs, const NumericOperator::Type nop, const double rhs, const NumericDict<>& nd );
    static bool eval( const double lhs, const NumericOperator::Type nop, const std::string rhs, const NumericDict<>& nd );
};



class NumericTerm {
    const std::string m_term;

    void parse();
public:
    typedef std::pair< NumericVariable*, NumericVariable* > variable_pair_t;
    typedef std::pair< variable_pair_t, NumericOperator::Type > term_pair_t;

    static term_pair_t parse_term( const std::string str );

    NumericVariable& variable_left;
    NumericVariable& variable_right;
    NumericOperator::Type op;

    NumericTerm( const std::string term );

    bool eval() const;
    bool eval( const NumericDict<NumericVariable>& nd ) const;

    const std::string string() const;
    const std::string term() const;
};


} // namespace truth
