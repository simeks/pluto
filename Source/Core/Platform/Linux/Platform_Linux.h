// Copyright 2008-2014 Simon Ekström

#ifndef _PLATFORM_LINUX_H
#define _PLATFORM_LINUX_H

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

#define API_EXPORT __attribute__((visibility("default")))
#define API_IMPORT

// String utilities for cross platform

template<size_t SIZE>
int strcpy_s(char (&dst)[SIZE], const char* src)
{
	strncpy(dst, src, SIZE);
	dst[SIZE-1] = '\0';
	return 0;
}

template<size_t SIZE>
int strcat_s(char (&dst)[SIZE], const char* src)
{
	strncat(dst, src, SIZE);
	dst[SIZE-1] = '\0';
	return 0;
}

INLINE int strcpy_s(char* dst, size_t size, const char* src)
{
	strncpy(dst, src, size);
	dst[size-1] = '\0';
	return 0;
}


INLINE void Sleep(int milliseconds)
{
	usleep(milliseconds * 1000);
}

#endif // _PLATFORM_LINUX_H
