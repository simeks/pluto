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
    Py_Initialize();
    PythonClass::ready_all();

    _kernel = new PlutoKernel();
    _window_manager = new WindowManager();
}
PlutoCore::~PlutoCore()
{
    ModuleManager::destroy();
}
void PlutoCore::initialize()
{
    ModuleManager::create();
}
void PlutoCore::install_python_module(PythonModule* module)
{
    _kernel->install_module(module);
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
