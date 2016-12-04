// Copyright 2008-2014 Simon Ekström

#ifndef _PLATFORM_WIN32_H
#define _PLATFORM_WIN32_H

#ifndef __NVCC__
#define ANALYSIS_ASSUME(expr) __analysis_assume(expr)
#else
#define ANALYSIS_ASSUME(expr)
#endif

#define DEBUG_BREAK __debugbreak()

#define INLINE __forceinline
#define OVERRIDE override
#define FINAL sealed
#define ABSTRACT abstract

#define ATTR_PRINTF(...)  

// Renaming functions for cross-platform compability
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define __thread __declspec( thread )

#define API_EXPORT __declspec( dllexport )
#define API_IMPORT __declspec( dllimport )

#endif // _PLATFORM_WIN32_H
