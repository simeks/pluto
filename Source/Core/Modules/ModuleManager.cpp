#include "Common.h"

#include "ModuleManager.h"

#include "Platform/FilePath.h"
#include "Platform/FileUtil.h"
#include "Platform/Process.h"

#if PLUTO_PLATFORM_WINDOWS
// TODO:
static const char* module_ext = ".dll";
#elif PLUTO_PLATFORM_LINUX
static const char* module_ext = ".so";
#endif
typedef ModuleInterface* (*CreateModuleFn)();

ModuleManager* ModuleManager::s_instance = nullptr;

ModuleManager& ModuleManager::instance()
{
    assert(s_instance);
    return *s_instance;
}
void ModuleManager::create()
{
    if (!s_instance)
    {
        s_instance = new ModuleManager();
    }
}
void ModuleManager::destroy()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}
ModuleManager::ModuleManager()
{
    add_module_directory("");
    add_module_directory(process::base_dir());
}
ModuleManager::~ModuleManager()
{
    unload_all();
}
ModuleInterface* ModuleManager::load_module(const std::string& name)
{
    auto it = _modules.find(name);
    if (it != _modules.end())
        return it->second.module_interface;

    std::string full_path;
    if (find_module_path(name, full_path))
    {
        Module module;
        module.handle = process::load_module(full_path.c_str());
        if (module.handle)
        {
            CreateModuleFn create_module = (CreateModuleFn)process::get_module_export(module.handle, "create_module");
            if (create_module)
            {
                module.module_interface = create_module();
                if (module.module_interface)
                {
                    module.module_interface->install();

                    _modules[name] = module;
                    return module.module_interface;
                }
                else
                {
                    // TODO:
                    fprintf(stderr, "Failed to load module '%s': 'create_module' returned null.\n", name.c_str());
                }
            }
            else
            {
                // TODO:
                fprintf(stderr, "Failed to load module '%s': Function 'create_module' not found.\n", name.c_str());
            }
        }
        else
        {
            // TODO:
            fprintf(stderr, "Failed to load module '%s'.\n", name.c_str());
        }
    }
    else
    {
        // TODO:
        fprintf(stderr, "Module '%s' not found.\n", name.c_str());
    }
    return nullptr;
}
void ModuleManager::unload_module(const std::string& name)
{
    auto it = _modules.find(name);
    if (it != _modules.end())
    {
        if (it->second.module_interface)
        {
            it->second.module_interface->uninstall();
            delete it->second.module_interface;
        }

        process::unload_module(it->second.handle);
        _modules.erase(it);
    }
}
void ModuleManager::unload_all()
{
    for (auto& it : _modules)
    {
        if (it.second.module_interface)
        {
            it.second.module_interface->uninstall();
            delete it.second.module_interface;
        }

        // Let the OS unload modules
        // process::unload_module(it.second.handle); 
    }
    _modules.clear();
}
void ModuleManager::add_module_directory(const std::string& dir)
{
    _module_directories.push_back(dir);
}
void ModuleManager::find_modules(const char* wildcard, std::vector<std::string>& modules) const
{
    for (auto& dir : _module_directories)
    {
        FilePath path(dir);
        path += PATH_SEPARATOR;
        path += wildcard;
        path += module_ext;

        std::vector<std::string> files;
        file_util::find_files(path.c_str(), files);
        for (auto& f : files)
        {
            FilePath fp(f);
            fp.trim_extension();
            modules.push_back(fp.filename());
        }

    }
}
void ModuleManager::loaded_modules(std::vector<ModuleInterface*>& modules) const
{
    for (auto& m : _modules)
    {
        modules.push_back(m.second.module_interface);
    }
}

bool ModuleManager::find_module_path(const std::string& module_name, std::string& module_path)
{
    for (auto& dir : _module_directories)
    {
        FilePath path(dir);
        path += PATH_SEPARATOR;
        path += module_name;
        path += module_ext;

        std::vector<std::string> files;
        file_util::find_files(path.c_str(), files);
        if(files.size())
        {
            module_path = dir + '/' + files[0];
            return true;
        }
    }
    return false;
}
