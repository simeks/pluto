#include <Core/Common.h>
#include <Core/Pluto/PlutoCore.h>
#include <Core/Python/PythonFunction.h>
#include <Core/Qt/WindowManager.h>

#include "VisModule.h"
#include "VisPythonModule.h"

PLUTO_IMPLEMENT_MODULE(VisModule);

VisModule* VisModule::s_instance = nullptr;

VisModule::VisModule()
{
    if (s_instance == nullptr)
        s_instance = this;
}
VisModule::~VisModule()
{
    if (s_instance == this)
        s_instance = nullptr;
}

void VisModule::install()
{
    VisPythonModule::create();
}
void VisModule::uninstall()
{
}
void VisModule::init()
{
}
VisModule& VisModule::instance()
{
    return *s_instance;
}

