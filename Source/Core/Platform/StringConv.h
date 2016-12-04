#ifndef __PLATFORM_STRING_CONV_H__
#define __PLATFORM_STRING_CONV_H__

#include <Core/API.h>

CORE_API bool convert_string(const char* src, wchar_t* dest, size_t dest_size);
CORE_API bool convert_string(const wchar_t* src, char* dest, size_t dest_size);

CORE_API bool convert_string(const std::string& src, std::wstring& dest);
CORE_API bool convert_string(const std::wstring& src, std::string& dest);

#endif // __PLATFORM_STRING_CONV_H__
