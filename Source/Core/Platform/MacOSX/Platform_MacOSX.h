// Copyright 2008-2014 Simon Ekström

#ifndef _PLATFORM_MACOSX_H
#define _PLATFORM_MACOSX_H

// Some macros and defines 

#define DEBUG_BREAK __builtin_trap()
#define ANALYSIS_ASSUME(expr)

#define INLINE inline __attribute__( ( always_inline ))
#define OVERRIDE override
#define FINAL sealed
#define ABSTRACT abstract

#if defined(__GNUC__) && !defined(_RELEASE)
	#define ATTR_PRINTF(...) __attribute__ ((format(printf, __VA_ARGS__)))
#else
	#define ATTR_PRINTF(...)  
#endif

// Max length of path name, from windows
#define MAX_PATH 260


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// type traits
#include <type_traits>


#endif // _PLATFORM_MACOSX_H
