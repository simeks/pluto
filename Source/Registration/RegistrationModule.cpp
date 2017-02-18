#include <Core/Common.h>
#include <Core/Python/Function.h>
#include <Core/Python/Module.h>

#include <Flow/FlowModule.h>
#include <Flow/FlowPin.h>

#include "RegistrationEngine.h"
#include "RegistrationModule.h"
#include "Transform.h"


class RegistrationPythonModule : public PythonModuleHelper<RegistrationPythonModule>
{
public:
    RegistrationPythonModule() {}
    ~RegistrationPythonModule() {}

    void post_init() OVERRIDE;

    Image transform(const Image& img, const Image& def);

    static const char* name()
    {
        return "registration";
    }

};
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2_RETURN(RegistrationPythonModule, transform, Image, Image);



void hey()
{
    std::cout << "hey" << std::endl;
}
void hey2(const char* a)
{
    std::cout << "hey2 " << a  << std::endl;
}
std::string hey3(const std::string& a)
{
    std::cout << "hey3 " << a << std::endl;
    return a;
}
int hey4(int a, int b)
{
    std::cout << "hey4 " << a << ", " << b << std::endl;
    return a+b;
}

PYTHON_MODULE(registration)
{
    PYTHON_MODULE_FUNCTION("hey", &hey);
    PYTHON_MODULE_FUNCTION("hey2", &hey2);
    PYTHON_MODULE_FUNCTION("hey3", &hey3);
    PYTHON_MODULE_FUNCTION("hey4", &hey4);

    PYTHON_MODULE_CLASS("RegistrationEngine", RegistrationEngine);
}

void RegistrationPythonModule::post_init()
{
    add_type("RegistrationEngine", RegistrationEngine::static_class());

    MODULE_ADD_PYTHON_FUNCTION(RegistrationPythonModule, transform,
        "transform(image, deformation)\n"
        "--\n"
        "Args:\n"
        "   image(Image) : Image to transform\n"
        "   deformation(Image) : Deformation field to apply\n");
}

Image RegistrationPythonModule::transform(const Image& img, const Image& def)
{
    return transform::transform(img, def);
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
