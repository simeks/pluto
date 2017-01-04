// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_FILEUTIL_H__
#define __FOUNDATION_FILEUTIL_H__

#ifdef PLUTO_PLATFORM_WINDOWS
#define PATH_SEPARATOR '\\'

#else
#define PATH_SEPARATOR '/'

#endif


/// @brief File system utilities
namespace file_util
{
    /// @brief Builds a full os-path from the given parameters 
    ///	@param base		Base path (E.g. "C:/Game/") 
    ///	@param relative	Relative path (E.g. "content/")
    CORE_API void build_os_path(std::string& out, const std::string& base, const std::string& relative);

    /// @brief Fixes all the slashes in a path 
    CORE_API void fix_slashes(char* path, char sep = PATH_SEPARATOR);

    /// @brief Fixes all the slashes in a path 
    CORE_API void fix_slashes(std::string& path, char sep = PATH_SEPARATOR);

    /// @brief Finds all files matching the specified patterns and puts them in the vector.
    CORE_API void find_files(const char* path, std::vector<std::string>& files);

    /// @brief Finds all directories matching the specified patterns and puts them in the vector.
    CORE_API void find_directories(const char* path, std::vector<std::string>& directories);

}; // namespace file_util



#endif // __FOUNDATION_FILEUTIL_H__
