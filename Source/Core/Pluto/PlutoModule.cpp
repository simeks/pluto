#include "Common.h"

#include "PlutoCore.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/Module.h"
#include "Python/StdStream.h"

namespace py = python;

namespace pluto
{
    std::string s_version = "0.1";
}

PYTHON_MODULE(pluto_api)
{
    py::def(module, "user_dir", &pluto::user_dir, "user_dir()");
    py::def(module, "python_dir", &pluto::python_dir, "python_dir()");
    py::def(module, "module_dir", &pluto::module_dir, "module_dir()");
    py::def(module, "run_file", &pluto::run_file, "run_file(file)");
    py::def(module, "register_class", &pluto::register_class, "register_class(cls)");
    py::def(module, "classes", &pluto::classes, "classes()");
    py::def(module, "create_object", &pluto::create_object, "create_object(cls, *args)");
    py::def(module, "auto_reload", &pluto::auto_reload, "auto_reload(module)");

    py::def(module, "Object", Object::static_class());
    py::def(module, "StdStream", PyStdStream::static_class());
    py::def(module, "__version__", pluto::s_version);
}
void pluto::install_python_module()
{
    PYTHON_MODULE_INSTALL(pluto_api);
}

const char* pluto::user_dir()
{
    return PlutoCore::instance().user_dir();
}
const char* pluto::python_dir()
{
    return PlutoCore::instance().python_dir();
}
const char* pluto::module_dir()
{
    return PlutoCore::instance().module_dir();
}
void pluto::run_file(const std::string& file)
{
    PlutoCore::instance().kernel()->run_file(file);
}
python::Object pluto::register_class(const python::Object& cls)
{
    if (PyType_Check(cls.ptr()) == 0)
        PYTHON_ERROR_R(TypeError, nullptr, "expected class");

    python::incref(cls);
    PythonClass::python_class((PyTypeObject*)cls.ptr());

    return cls;
}
python::Object pluto::classes()
{
    std::vector<PythonClass*> classes = PythonClass::classes();
    PyObject* list = PyList_New(classes.size());
    for (size_t i = 0; i < classes.size(); ++i)
    {
        PyList_SetItem(list, i, python_convert::to_python(classes[i]->name()));
    }
    return list;
}
Object* pluto::create_object(const Tuple& args)
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
void pluto::auto_reload(const python::Object& module)
{
    PlutoCore::instance().kernel()->add_auto_reload(module);
}

