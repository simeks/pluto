#include "Common.h"

#include "../Process.h"
#include "../FileUtil.h"

#include <dlfcn.h>

void* process::load_module(const char* file)
{
    return dlopen(file, RTLD_LAZY);
}
void process::unload_module(void* handle)
{
    dlclose(handle);
}
void* process::get_module_export(void* handle, const char* proc_name)
{
    assert(handle);
    return dlsym(handle, proc_name);
}
const char* process::base_dir()
{
    static char dir[1024] = "";
    if (!(*dir))
    {
        ssize_t len = readlink("/proc/self/exe", dir, 1023);
        assert(len != -1);

        if (len)
        {
            --len;
            for (; len > 0; --len)
            {
	        if (dir[len - 1] == '/' || dir[len - 1] == '\\')
                {
                    break;
                }
            }
            dir[len] = '\0';
        }
        file_util::fix_slashes(dir, '/');
        
    }
    return dir;
}

