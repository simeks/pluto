// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "../FileUtil.h"
#include "Platform/FilePath.h"

#include <dirent.h>
#include <fnmatch.h>
#include <sys/types.h>

void file_util::find_files(const char* path, std::vector<std::string>& files)
{
    FilePath fp(path);
   
    DIR* dir = opendir(fp.directory().c_str());
    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type != DT_DIR && fnmatch(fp.filename().c_str(), entry->d_name, 0) == 0)
            {
                files.push_back(entry->d_name);
            }
        }
        closedir(dir);
    }
}

void file_util::find_directories(const char* path, std::vector<std::string>& directories)
{
    FilePath fp(path);

    DIR* dir = opendir(path);
    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_DIR && fnmatch(fp.filename().c_str(), entry->d_name, 0) == 0)
            {
                directories.push_back(entry->d_name);
            }
        }
        closedir(dir);
    }
}


