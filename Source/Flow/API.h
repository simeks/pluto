#ifndef __FLOW_API_H__
#define __FLOW_API_H__

#include <Core/Platform/Platform.h>

#ifdef PLUTO_FLOW_EXPORTS
#define FLOW_API API_EXPORT
#else
#define FLOW_API API_IMPORT
#endif

#endif // __FLOW_API_H__
