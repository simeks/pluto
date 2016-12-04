#include "Common.h"

#include "StringConv.h"

#ifdef PLUTO_PLATFORM_WINDOWS
#include "Platform/WindowsWrapper.h"

bool convert_string(const char* src, wchar_t* dest, size_t dest_size)
{
    return MultiByteToWideChar(CP_UTF8, 0, src, -1, dest, (int)dest_size) != 0;
}
bool convert_string(const wchar_t* src, char* dest, size_t dest_size)
{
    return WideCharToMultiByte(CP_UTF8, 0, src, -1, dest, (int)dest_size, NULL, NULL) != 0;
}

bool convert_string(const std::string& src, std::wstring& dest)
{
    size_t dest_length = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, NULL, 0);
    dest.resize(dest_length-1);
    return convert_string(src.c_str(), (wchar_t*)dest.data(), dest_length);
}
bool convert_string(const std::wstring& src, std::string& dest)
{
    size_t dest_length = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, NULL, 0, NULL, NULL);
    dest.resize(dest_length - 1);
    return convert_string(src.c_str(), (char*)dest.data(), dest_length);
}

#endif
