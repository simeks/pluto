#include <Core/Common.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>

#include <Flow/FlowModule.h>
#include <Flow/FlowPin.h>

#include "RegistrationEngine.h"
#include "RegistrationModule.h"
#include "Transform.h"

namespace py = python;
PYTHON_MODULE(registration)
{
    py::def(module, "transform", &transform::transform);
    py::def(module, "RegistrationEngine", RegistrationEngine::static_class());
}


PLUTO_IMPLEMENT_MODULE(RegistrationModule);

RegistrationModule::RegistrationModule()
{
}
RegistrationModule::~RegistrationModule()
{
}
void RegistrationModule::install()
{
    PYTHON_MODULE_INSTALL(registration);
}
