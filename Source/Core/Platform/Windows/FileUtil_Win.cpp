// Copyright 2008-2014 Simon Ekström

#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../FileUtil.h"


void file_util::find_files(const char* path, std::vector<std::string>& files)
{
    std::wstring wpath;
    convert_string(path, wpath);

    WIN32_FIND_DATA fd;
    HANDLE fh;

    fh = FindFirstFile(wpath.c_str(), &fd);
    if (fh == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // Not a directory
        {
            std::string f;
            convert_string(fd.cFileName, f);
            files.push_back(f);
        }

    } while (FindNextFile(fh, &fd) != 0);

    FindClose(fh);
}

void file_util::find_directories(const char* path, std::vector<std::string>& directories)
{
    std::wstring wpath;
    convert_string(path, wpath);

    WIN32_FIND_DATA fd;
    HANDLE fh;

    fh = FindFirstFile(wpath.c_str(), &fd);
    if (fh == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if ((wcscmp(fd.cFileName, TEXT("..")) != 0) && (wcscmp(fd.cFileName, TEXT(".")) != 0))
            {
                std::string f;
                convert_string(fd.cFileName, f);
                directories.push_back(f);
            }
        }

    } while (FindNextFile(fh, &fd) != 0);

    FindClose(fh);
}


