#ifndef __VIS_API_H__
#define __VIS_API_H__

#include <Core/Platform/Platform.h>

#ifdef PLUTO_VIS_EXPORTS
#define VIS_API API_EXPORT
#else
#define VIS_API API_IMPORT
#endif

#endif // __VIS_API_H__
