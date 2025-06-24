// Copyright 2025 The OnePointer Authors.
//

#pragma once

#include <functional>






namespace oneptr {

template< bool B >
class EnableIf {
    const bool m_condition;
public:
#if B
    EnableIf( bool condition, void (*f)() )
        :   m_condition( condition )
    {
        #if m_condition
            f();
        #endif
    }
    template< typename... Args >
    EnableIf( bool condition, void (*f)(Args&&...), Args&&... args )
        :   m_condition( condition )
    {
        #if m_condition
            f( args );
        #endif
    }
#endif
    void operator()() {}

    template< typename... Args >
    void operator()( bool condition, void (*f)(Args&&...), Args&&... args ) {}
};


} // namespace oneptr

