// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <memory>

#include "mini/ini.h"
#include "boolexpr.hpp"
#include "bool_value_dict.hpp"


namespace truth {


/// @brief The datastructure to handle *.ini files
class Ini
    :   public mINI::INIStructure
{
protected:
    /// @brief The file currently in use
    mINI::INIFile& m_file;

    
public:
    /// @brief A section iterator, for iterating over `[section_name]` sections
    typedef mINI::INIStructure::const_iterator section_iterator;
    /// @brief The datatype a section has
    typedef mINI::INIMap< std::string > ini_map_t;
    /// @brief The datatype a section has
    typedef ini_map_t section_t;
    /// @brief A iterator over `(key, value)` pairs inside of a section
    typedef ini_map_t::const_iterator ini_map_iterator;

    /// @brief Constructor
    Ini();

    /// @brief Create a new ini from a file
    /// @param path The path to read the file from
    /// @return A reference to a newly-created `Ini` object
    static Ini& readFile( const char* path );
    /// @brief Read an ini from a file
    /// @param path The path to read the file from
    /// @return A reference to `*this`
    Ini& fromFile( const char* path );

#ifdef TRUTH_HAVE_INI_WRITE
    /// @brief Write everything pretty and readable to a file at path
    /// @param path The path to write the file to
    /// @return True on success
    bool write( const char* path );
#endif

    /// @brief Get a value by searching for a section's key
    /// @param section The section identifier
    /// @param key The key of the section that thus has this value
    /// @return A reference to the key's value or an empty string, if the value did not exist yet.
    std::string& getPath( const std::string section, const std::string key );
    /// @brief The path extension to *.ini files gets values by `section.varname`.
    /// @param section_path The path in the format `section_name.var_name`.
    /// @return A reference to the key's value or an empty string, if the value did not exist yet.
    std::string& getPath( const std::string section_path );

    /// @brief Write the ini variables to a `BoolDict<BoolValue>`, if a key's value is parsable to `BoolValue`
    /// @return A reference to a newly-created bool dict.
    BoolDict<BoolValue>& toDict() const;
    /// @brief Write the ini variables to a `BoolDict<BoolValue>`, if a key's value is parsable to `BoolValue`
    /// @param bd The reference to a bool dict to add the values of this ini to
    /// @return A reference to `bd`
    BoolDict<BoolValue>& toDict( BoolDict<BoolValue>& bd ) const;

    /// @brief Get a bool expression from the ini file
    /// @param section The section to lookup in
    /// @param key The key to lookup
    /// @return A reference to a newly-created `BoolExpr`. If `(*this)[section][key]` is no algebraic-logic expression,
    ///   the `length() == 0` of the returned `BoolExpr`
    BoolExpr& getExpr( const char* section, const char* key ) const;
};


/// @brief A simple ini singleton
class IniS {
protected:
    static std::unique_ptr< Ini > ms_instance;

    IniS();
    IniS( IniS& other ) = delete;

public:
    /// @brief Get the instance of the `Ini` object and create the instance, if not yet there.
    /// @return A reference to the instance of this singleton
    static Ini& getInstance();
    /// @brief Read another file
    /// @param filepath The path to the file to read
    /// @return A reference to the instance of this singleton
    static const Ini& readFile( const char* filepath );
    /// @brief Get a reference to the instance.
    /// @return A reference to the instance of this singleton 
    static const Ini& get();
};


} // namespace truth

/// @brief Ini-instance macro. Get the default mINI instance of this library
#define ini truth::IniS::get()
/// @brief Get a `BoolDict<BoolValue>` from the default mINI instance of this library
#define ini_dict truth::IniS::get().toDict()
/// @brief Read anotherfile with the mINI instance
#define ini_read_file(path) truth::IniS::readFile(#path)
