// Copyright 2008-2014 Simon Ekström


#ifndef _PLATFORM_H
#define _PLATFORM_H

#ifdef PLUTO_PLATFORM_WINDOWS
#include "Windows/Platform_Win.h"
#elif PLUTO_PLATFORM_MACOSX
#include "MacOSX/Platform_MacOSX.h"
#elif PLUTO_PLATFORM_LINUX
#include "Linux/Platform_Linux.h"
#endif

#include <Core/API.h>

namespace platform
{
    CORE_API void set_utf8_output();
}


#endif // _PLATFORM_H
