#include <Core/Common.h>
#include <Core/Python/PythonModule.h>

#include <Flow/FlowModule.h>
#include <Flow/FlowPin.h>

#include "RegistrationEngine.h"
#include "RegistrationModule.h"
#include "Registration.h"
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
//void RegistrationModule::init()
//{
//    //FlowPinDef transform_pins[] = {
//    //    { "Source", FlowPin::In, "" },
//    //    { "Deformation", FlowPin::In, "" },
//    //    { "Out", FlowPin::Out, "" },
//    //    { 0, 0, 0 }
//    //};
//
//    //FlowNodeDef transform_node;
//    //transform_node.title = "Transform";
//    //transform_node.category = "Registration";
//    //transform_node.class_name = "registration.Transform";
//    //transform_node.doc = "";
//    //transform_node.pins = transform_pins;
//    //transform_node.fn = transform::transform_node;
//
//    //FlowModule::instance().install_node_template(transform_node);
//
//    //FlowPinDef registration_pins[] = {
//    //    { "Fixed0", FlowPin::In, "" },
//    //    { "Moving0", FlowPin::In, "" },
//
//    //    { "Fixed1", FlowPin::In, "" },
//    //    { "Moving1", FlowPin::In, "" },
//
//    //    { "Fixed2", FlowPin::In, "" },
//    //    { "Moving2", FlowPin::In, "" },
//
//    //    { "ConstraintMask", FlowPin::In, "" },
//    //    { "ConstraintValues", FlowPin::In, "" },
//
//    //    { "StartingGuess", FlowPin::In, "" },
//
//    //    { "Deformation", FlowPin::Out, "" },
//    //    { 0, 0, 0 }
//    //};
//
//    //FlowNodeDef registration_node;
//    //registration_node.title = "Registration";
//    //registration_node.category = "Registration";
//    //registration_node.class_name = "registration.Registration";
//    //registration_node.doc = "";
//    //registration_node.pins = registration_pins;
//    //registration_node.fn = registration::registration_node;
//
//    //FlowModule::instance().install_node_template(registration_node);
//}
