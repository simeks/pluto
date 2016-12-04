// Copyright 2008-2014 Simon Ekström

#ifndef _TIMER_H
#define _TIMER_H


namespace timer
{
    CORE_API void initialize();

    /// Returns the applications current tick count.
    CORE_API uint64_t tick_count();

    /// Returns elapsed seconds since timer initialization.
    CORE_API double seconds();


};




#endif // _TIMER_H
