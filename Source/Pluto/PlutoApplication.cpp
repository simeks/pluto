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
}
PlutoApplication::~PlutoApplication()
{
    _kernel->stop();
    delete _kernel;

    PlutoCore::instance().shutdown();
    PlutoCore::destroy();
}
void PlutoApplication::init()
{
    ModuleManager::instance().load_module("Flow");

    PlutoCore::instance().init();
    _kernel = new PlutoKernelProxy(PlutoCore::instance().kernel());
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
