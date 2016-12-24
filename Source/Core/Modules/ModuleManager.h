#ifndef __CORE_MODULE_MANAGER_H__
#define __CORE_MODULE_MANAGER_H__

#include "ModuleInterface.h"

class CORE_API ModuleManager
{
public:
    ModuleManager();
    ~ModuleManager();

    ModuleInterface* load_module(const std::string& name);
    void unload_module(const std::string& name);
    void unload_all();

    void add_module_directory(const std::string& dir);

    void find_modules(const char* wildcard, std::vector<std::string>& modules) const;

    static ModuleManager& instance();
    static void create();
    static void destroy();

private:
    struct Module
    {
        std::string full_path;
        void* handle;
        ModuleInterface* module_interface;
    };
    
    bool find_module_path(const std::string& module_name, std::string& module_path);

    static ModuleManager* s_instance;

    std::vector<std::string> _module_directories;
    std::map<std::string, Module> _modules;
};


#endif // __CORE_MODULE_MANAGER_H__
