#include <Core/Common.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>

#include <Flow/FlowModule.h>
#include <Flow/FlowPin.h>

#include "RegistrationEngine.h"
#include "RegistrationModule.h"
#include "Transform.h"

PYTHON_MODULE(registration)
{
    PYTHON_MODULE_FUNCTION("transform", &transform::transform);
    PYTHON_MODULE_CLASS("RegistrationEngine", RegistrationEngine);
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
