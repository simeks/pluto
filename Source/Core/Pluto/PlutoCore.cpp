#include "Common.h"

#include "Modules/ModuleManager.h"
#include "Object/PythonClass.h"
#include "PlutoCore.h"
#include "PlutoKernel.h"
#include "Python/PythonWrapper.h"
#include "Qt/WindowManager.h"

PlutoCore* PlutoCore::s_instance = nullptr;

PlutoCore::PlutoCore(int, char**)
{
    ModuleManager::create();

    _kernel = new PlutoKernel();
    _window_manager = new WindowManager();
}
PlutoCore::~PlutoCore()
{
    delete _window_manager;
    delete _kernel;

    ModuleManager::destroy();

    Py_Finalize();
}
void PlutoCore::init()
{
    _kernel->prepare();

    std::vector<ModuleInterface*> modules;
    ModuleManager::instance().loaded_modules(modules);
    for (auto m : modules)
    {
        m->init();
    }
}
void PlutoCore::shutdown()
{
    std::vector<ModuleInterface*> modules;
    ModuleManager::instance().loaded_modules(modules);
    for (auto m : modules)
    {
        m->shutdown();
    }
}
const char* PlutoCore::user_dir() const
{
    return "..\\User"; // TODO:
}
PlutoKernel* PlutoCore::kernel() const
{
    return _kernel;
}
WindowManager* PlutoCore::window_manager() const
{
    return _window_manager;
}
PlutoCore& PlutoCore::instance()
{
    return *s_instance;
}
void PlutoCore::create(int argc, char** argv)
{
    if (!s_instance)
    {
        s_instance = new PlutoCore(argc, argv);
    }
}
void PlutoCore::destroy()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}
