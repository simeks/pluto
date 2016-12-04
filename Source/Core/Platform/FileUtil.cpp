// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FileUtil.h"


//-------------------------------------------------------------------------------
void file_util::build_os_path(std::string& out, const std::string& base, const std::string& relative)
{
    out.clear();
    out = base;

    int i = (uint32_t)base.size() - 1;
    if (out[i] != '\\' && out[i] != '/')
        out += '/';

    out += relative;
    fix_slashes(out);
}
//-------------------------------------------------------------------------------
void file_util::fix_slashes(char* path, char sep)
{
    while (*path)
    {
        if (*path == '\\' || *path == '/')
        {
            *path = sep;
        }
        path++;
    }
}
void file_util::fix_slashes(std::string& path, char sep)
{
    char* _path = &path[0];
    while (*_path)
    {
        if (*_path == '\\' || *_path == '/')
        {
            *_path = sep;
        }
        _path++;
    }
}


//-------------------------------------------------------------------------------



