#ifndef __PLATFORM_PROCESS_H__
#define __PLATFORM_PROCESS_H__


namespace process
{
    CORE_API void* load_module(const char* file);
    CORE_API void unload_module(void* handle);
    CORE_API void* get_module_export(void* handle, const char* name);

    CORE_API const char* base_dir();
}

#endif // __PLATFORM_PROCESS_H__
