#include <Core/Common.h>
#include <Core/Python/PythonModule.h>

#include <Flow/FlowModule.h>
#include <Flow/FlowPin.h>

#include "RegistrationEngine.h"
#include "RegistrationModule.h"
#include "Transform.h"

namespace registration
{
    Image transform(const Image& img, const Image& def);
}

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

namespace python_module
{
    //void add_function();
    //void add_object();
    //void add_type();

} // namespace python_module

namespace python_function
{
    template<typename Fn>
    PyObject* create_function(Fn fn)
    {

    }


} // namespace python_function


#define PYTHON_MODULE(Name) \
static struct PyModuleDef s_##Name##_module_def = { \
    PyModuleDef_HEAD_INIT, \
    #Name,   /* m_name */ \
    "", /* m_doc */ \
    -1,       /* m_size */ \
    0, /* m_methods */ \
    0,  /* m_reload */ \
    0, /* m_traverse */ \
    0, /* m_clear */ \
    0,  /* m_free */ \
}; \
void init_module_##Name##(); \
PyObject* PyInit_##Name##() \
{ \
    PyObject* m = PyModule_Create(&s_##Name##_module_def); \
    if (!m) \
        PyErr_Print(); \
    return m; \
} \
void init_module_##Name##(PyObject* m)


#define PYTHON_MODULE_INSTALL(name) \
    extern PyObject* PyInit_##name##(); \
    PyImport_AppendInittab(#name, PyInit_##name##);

#define PYTHON_MODULE_FUNCTION(name, fn)
#define PYTHON_MODULE_TYPE(name, cls)

PYTHON_MODULE(registration)
{
    PYTHON_MODULE_TYPE("RegistrationEngine", RegistrationEngine::static_class());
    PYTHON_MODULE_FUNCTION("transform", registration::transform);
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
