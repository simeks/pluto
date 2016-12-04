#include "Common.h"
#include "Platform/WindowsWrapper.h"

#include "../Process.h"
#include "../FileUtil.h"

void* process::load_module(const char* file)
{
    std::wstring wfile;
    convert_string(file, wfile);

    return LoadLibrary(wfile.c_str());
}
void process::unload_module(void* handle)
{
    FreeLibrary((HMODULE)handle);
}
void* process::get_module_export(void* handle, const char* proc_name)
{
    assert(handle);
    return GetProcAddress((HMODULE)handle, proc_name);
}
const char* process::base_dir()
{
    static char dir[1024] = "";
    if (!(*dir))
    {
        // TODO: HINSTANCE?
        if (::GetModuleFileNameA(NULL, dir, 1024))
        {
            file_util::fix_slashes(dir, '/');
            size_t len = strlen(dir);

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
        }
    }
    return dir;
}

