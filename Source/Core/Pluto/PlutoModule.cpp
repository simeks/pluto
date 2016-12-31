#include "Common.h"

#include "Image/ImageObject.h"
#include "PlutoCore.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/PythonFunction.h"
#include "Python/StdStream.h"

PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(PlutoModule, user_dir);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1(PlutoModule, print_html, std::string);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS1_RETURN(PlutoModule, register_class, PyObject*);
PYTHON_FUNCTION_WRAPPER_CLASS_ARGS0_RETURN(PlutoModule, classes);
PYTHON_FUNCTION_WRAPPER_CLASS_TUPLE_RETURN(PlutoModule, create_object);

std::string PlutoModule::s_version = "0.0";

PlutoModule::PlutoModule()
{
}
PlutoModule::~PlutoModule()
{
}
void PlutoModule::post_init()
{
    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, user_dir, "");
    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, print_html, "");
    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, register_class, "");
    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, classes, "");
    MODULE_ADD_PYTHON_FUNCTION(PlutoModule, create_object, "");

    add_type("Object", Object::static_class());
    add_type("StdStream", PyStdStream::static_class());
    add_object("__version__", python_convert::to_python(s_version));
}
const char* PlutoModule::user_dir()
{
    return PlutoCore::instance().user_dir();
}
void PlutoModule::print_html(const std::string& txt)
{
    PyStdStream* htmlout = object<PyStdStream>("htmlout");
    if (htmlout)
        htmlout->write(txt.c_str());
}
PyObject* PlutoModule::register_class(PyObject* cls)
{
    if (PyType_Check(cls) == 0)
        PYTHON_ERROR_R(TypeError, nullptr, "expected class");

    Py_INCREF(cls);
    PythonClass::python_class((PyTypeObject*)cls);

    return cls;
}
PyObject* PlutoModule::classes() const
{
    std::vector<PythonClass*> classes = PythonClass::classes();
    PyObject* list = PyList_New(classes.size());
    for (size_t i = 0; i < classes.size(); ++i)
    {
        PyList_SetItem(list, i, python_convert::to_python(classes[i]->name()));
    }
    return list;
}
Object* PlutoModule::create_object(const Tuple& args)
{
    if (args.size() < 1)
        PYTHON_ERROR_R(AttributeError, nullptr, "expected at least one argument");

    const char* class_name = python_convert::from_python<const char*>(args.get(0));
    if (!class_name)
        PYTHON_ERROR_R(AttributeError, nullptr, "expected first argument to be string");


    PythonClass* cls = PythonClass::python_class(class_name);
    if (!cls)
        PYTHON_ERROR_R(ValueError, nullptr, "no class found");

    Tuple obj_args(args.size() - 1);
    for (int i = 1; i < args.size(); ++i)
        obj_args.set(i-1, args.get(i));

    return cls->create_object(obj_args);
}
const char* PlutoModule::name()
{
    return "pluto_api";
}
