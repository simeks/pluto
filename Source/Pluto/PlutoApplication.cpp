#include <Core/Common.h>
#include <Core/Modules/ModuleManager.h>
#include <Core/Pluto/PlutoCore.h>

#include "PlutoApplication.h"
#include "PlutoKernelProxy.h"

PlutoApplication::PlutoApplication(int argc, char** argv) : 
    QApplication(argc, argv)
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    PlutoCore::create(argc, argv);
    _kernel = new PlutoKernelProxy(PlutoCore::instance().kernel());
}
PlutoApplication::~PlutoApplication()
{
    delete _kernel;
    PlutoCore::destroy();
}
void PlutoApplication::init()
{
    _kernel->prepare();
    PlutoCore::instance().initialize();
    ModuleManager::instance().load_module("Flow");
}
PlutoKernelProxy* PlutoApplication::kernel() const
{
    return _kernel;
}
PlutoApplication& PlutoApplication::instance()
{
    return *((PlutoApplication*)QCoreApplication::instance());
}
void PlutoApplication::start_kernel()
{
    QMetaObject::invokeMethod(_kernel, "start");
}
