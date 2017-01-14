#include <Core/Common.h>
#include <Core/Python/PythonModule.h>

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

    ImageObject* transform(ImageObject* img, ImageObject* def);

    static const char* name()
    {
        return "registration";
    }

};
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2_RETURN(RegistrationPythonModule, transform, ImageObject*, ImageObject*);

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

ImageObject* RegistrationPythonModule::transform(ImageObject* img, ImageObject* def)
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
    RegistrationPythonModule::create();
}
