#include "Common.h"

#include "Object/Object.h"
#include "PlutoCore.h"
#include "PlutoKernel.h"
#include "PlutoModule.h"
#include "Python/Module.h"
#include "Python/StdStream.h"

namespace py = python;

namespace pluto
{
    std::string _version = "0.1";
    bool _auto_reload_enabled = false;
}

PYTHON_MODULE(_pluto)
{
    module.def("user_dir", &pluto::user_dir, "user_dir()");
    module.def("python_dir", &pluto::python_dir, "python_dir()");
    module.def("module_dir", &pluto::module_dir, "module_dir()");
    module.def("run_file", &pluto::run_file, "run_file(file)");
    module.def("register_class", &pluto::register_class, "register_class(cls)");
    module.def("create_object", &pluto::create_object, "create_object(cls, *args)");
    module.def("auto_reload", &pluto::auto_reload, "auto_reload(module)");

    module.def("Object", Object::static_class());
    module.def("__version__", pluto::_version);

    module.def("StdStream", python::Stream::stream_class());
}
void pluto::install_python_module()
{
    PYTHON_MODULE_INSTALL(_pluto);
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
        PYTHON_ERROR(PyExc_TypeError, "expected class");

    PythonClass::python_class((PyTypeObject*)python::incref(cls.ptr()));

    return cls;
}
Object* pluto::create_object(const Tuple& args)
{
    if (args.size() < 1)
        PYTHON_ERROR(PyExc_AttributeError, "expected at least one argument");

    const char* class_name = python::from_python<const char*>(args.get(0).ptr());
    if (!class_name)
        PYTHON_ERROR(PyExc_AttributeError, "expected first argument to be string");


    PythonClass* cls = PythonClass::python_class(class_name);
    if (!cls)
        PYTHON_ERROR(PyExc_ValueError, "no class found");

    Tuple obj_args(args.size() - 1);
    for (int i = 1; i < args.size(); ++i)
        obj_args.set(i-1, args.get(i));

    return cls->create_object(obj_args);
}
void pluto::auto_reload(const python::Object& module)
{
    if (_auto_reload_enabled)
        PlutoCore::instance().kernel()->add_auto_reload(module);
}
void pluto::enable_auto_reload(bool enable)
{
    _auto_reload_enabled = enable;
}

