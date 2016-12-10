#include "Common.h"

#include "PythonFunction.h"
#include "PythonCommon.h"

PythonFunction::PythonFunction(
    void* self, 
    const char* name, 
    PyCFunction meth, 
    int flags, 
    const char* doc)
{
    _def.ml_name = name;
    _def.ml_meth = meth;
    _def.ml_flags = flags;
    _def.ml_doc = doc;

    _fn = (PyCFunctionObject*)PyCFunction_New(&_def, PyCapsule_New(self, nullptr, nullptr));
    if (!_fn)
        PyErr_Print();
}
PythonFunction::~PythonFunction()
{
    Py_XDECREF(_fn);
}
PyObject* PythonFunction::object()
{
    return (PyObject*)_fn;
}
