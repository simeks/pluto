#include "Common.h"

#include "Module.h"
#include "Object/PythonClass.h"
#include "PythonCommon.h"

namespace python
{
    Module::Module(PyObject* obj) : Object(obj)
    {
        assert(PyModule_Check(obj));
    }
    Module::Module(Borrowed obj) : Object(obj)
    {
        assert(PyModule_Check((PyObject*)obj));
    }
    void Module::def(const char* name, PythonClass* cls)
    {
        python::setattr(*this, name, Borrowed((PyObject*)cls->python_type()));
    }

    Object import(const char* name)
    {
        PyObject* m = PyImport_ImportModule(name);
        if (!m)
        {
            PyErr_Print(); // TODO: Error handling
            return Object();
        }
        return m;
    }
    Object reload_module(const Object& module)
    {
        PyObject* m = PyImport_ReloadModule(module.ptr());
        if (!m)
        {
            PyErr_Print();
            return Object();
        }
        return m;
    }
    Object get_dict(const Object& module)
    {
        return Object(Borrowed(PyModule_GetDict(module.ptr())));
    }
}
