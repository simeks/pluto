// Copyright 2008-2014 Simon Ekström

#ifndef _ASSERT_H
#define _ASSERT_H

namespace assert_util
{
    enum AssertRetval
    {
        RET_DEBUGGER = 0,
        RET_CONTINUE,
        RET_ABORT,
    };

    CORE_API AssertRetval util_assert_msg(const char* msg, const char* file_name, unsigned int line);
}

#ifdef assert
#undef assert
#endif

#if defined(PLUTO_BUILD_DEBUG)
#define assert( _exp ) \
    if(!(_exp)) \
    { \
        if (assert_util::util_assert_msg("Assertion Failed: " #_exp, __FILE__, __LINE__) == assert_util::RET_DEBUGGER) \
        { \
            DEBUG_BREAK; \
        } \
    } \
    ANALYSIS_ASSUME(_exp);

#define assert_msg( _exp, _msg ) \
    if(!(_exp)) \
    { \
        if (assert_util::util_assert_msg(_msg, __FILE__, __LINE__) == assert_util::RET_DEBUGGER) \
        { \
            DEBUG_BREAK; \
        } \
    } \
    ANALYSIS_ASSUME(_exp);

#define verify( _exp ) assert( _exp )

#else

#define assert( _exp )              ((void)(_exp))
#define assert_msg( _exp, _msg )    ((void)(_exp))
#define verify( _exp)               (_exp)


#endif




#endif // _ASSERT_H
