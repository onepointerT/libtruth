// Copyright 2025 The OnePointer Authors.
//

#include "truth_ini.hpp"

namespace truth {


Ini::Ini()
    :   mINI::INIStructure()
    ,   m_file( *new mINI::INIFile( "" ) )
{}


Ini& Ini::readFile( const char* path ) {
    Ini* inistruct = new Ini();
    return inistruct->fromFile( path );
}


Ini& Ini::fromFile( const char* path ) {
    this->m_file = *new mINI::INIFile( path );
    this->m_file.read( *this );
    return *this;
}

#ifdef TRUTH_HAVE_INI_WRITE
bool Ini::write( const char* path ) {
    return this->m_file.write( *this, true );
}
#endif


BoolDict<BoolValue>& Ini::toDict() const {
    BoolDict<BoolValue>* bd = new BoolDict<BoolValue>();
    return this->toDict( *bd );
}

BoolDict<BoolValue>& Ini::toDict( BoolDict<BoolValue>& bd ) const {
    for ( section_iterator si : { begin(), end() } ) {
        for ( ini_map_iterator im : { si->second.begin(), si->second.end() } ) {
            if ( BoolValue::is_parsable(im->second.c_str())) {
                bd[im->first] = BoolValue::parse( im->second.c_str() );
            }
        }
    }
    return bd;
}

BoolExpr& Ini::getExpr( const char* section, const char* key ) const {
    if ( this->has(section) ) {
        ini_map_t im = this->get( section );
        if ( im.has( key ) ) {
            return *new BoolExpr( im.get( key ) );
        }
    }
    return *new BoolExpr("");
}


std::unique_ptr< Ini > IniS::ms_instance = std::unique_ptr< Ini >();


IniS::IniS()
{}


Ini& IniS::getInstance() {
    if ( ! ms_instance ) {
        ms_instance.reset( new Ini() );
    }
    return *ms_instance;
}

const Ini& IniS::readFile( const char* filepath ) {
    return getInstance().fromFile( filepath );
}

const Ini& IniS::get() {
    return getInstance();
}


} // namespace truth