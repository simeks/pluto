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

    Image transform(const Image& img, const Image& def);

    static const char* name()
    {
        return "registration";
    }

};
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS2_RETURN(RegistrationPythonModule, transform, Image, Image);


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
void init_module_##Name##(PyObject* m); \
PyObject* PyInit_##Name##() \
{ \
    PyObject* m = PyModule_Create(&s_##Name##_module_def); \
    if (!m) \
        PyErr_Print(); \
    else \
        init_module_##Name##(m); \
    return m; \
} \
void init_module_##Name##(PyObject* m)


#define PYTHON_MODULE_INSTALL(name) \
    extern PyObject* PyInit_##name##(); \
    PyImport_AppendInittab(#name, PyInit_##name##);

#define PYTHON_MODULE_FUNCTION(name, fn)
#define PYTHON_MODULE_TYPE(name, cls)

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
    m;

    PyObject* fn = python::make_function(&hey);

    int a = PyObject_SetAttrString(m, "hey", fn);
    a;
    PyObject_SetAttrString(m, "hey2", python::make_function(&hey2));
    PyObject_SetAttrString(m, "hey3", python::make_function(&hey3));
    PyObject_SetAttrString(m, "hey4", python::make_function(&hey4));

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
