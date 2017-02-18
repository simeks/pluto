#include "Common.h"

#include "Module.h"
#include "PythonCommon.h"

namespace python
{
    Module::Module(PyObject* m) : Object(m)
    {
    }

    Object import(const char* name)
    {
        PyObject* m = PyImport_ImportModule(name);
        if (!m)
        {
            PyErr_Print(); // TODO: Error handling
            return Object();
        }
        Module ret(m);
        Py_DECREF(m);
        return ret;
    }
    Object reload_module(const Object& module)
    {
        PyObject* m = PyImport_ReloadModule(module.ptr());
        if (!m)
        {
            PyErr_Print();
            return Object();
        }
        Module ret(m);
        Py_DECREF(m);
        return ret;
    }
    Object get_dict(const Object& module)
    {
        return Object(PyModule_GetDict(module.ptr()));
    }
}
