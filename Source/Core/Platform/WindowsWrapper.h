#ifndef __WINDOWS_WRAPPER_H__
#define __WINDOWS_WRAPPER_H__

#ifdef PLUTO_PLATFORM_WINDOWS
#if defined(_WINDOWS_)
#error "<windows.h> has been included by other means than WindowsWrapper.h"
#endif

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#define NODRAWTEXT // DrawText()

#include <windows.h>

// Undefine annoying windows macros
#undef min
#undef max
#undef MB_RIGHT
#endif

#endif // __WINDOWS_WRAPPER_H__
