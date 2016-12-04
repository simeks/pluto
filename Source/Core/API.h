#ifndef __CORE_API_H__
#define __CORE_API_H__

#ifdef PLUTO_CORE_EXPORTS
#define CORE_API API_EXPORT
#else
#define CORE_API API_IMPORT
#endif

#endif // __CORE_API_H__
