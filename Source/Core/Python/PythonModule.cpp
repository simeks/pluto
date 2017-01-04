#include "Common.h"

#include <Object/Object.h>

#include "PythonCommon.h"
#include "PythonFunction.h"
#include "PythonModule.h"

PythonModule::PythonModule(PyObject* module) : _module(module)
{
    Py_XINCREF(_module);
}
PythonModule::PythonModule(const char* name) : _module(nullptr)
{
    memset(&_module_def, 0, sizeof(_module_def));
    _module_def.m_name = name;
}
PythonModule::~PythonModule()
{
    for (auto f : _functions)
        delete f;
    _functions.clear();

    Py_XDECREF(_module);
}
void PythonModule::add_function(const char *name, PyCFunction fn, int flags, const char *doc)
{
    PythonFunction* pyfn = new PythonFunction(this, name, fn, flags, doc);
    _functions.push_back(pyfn);
    set_object(name, pyfn->object());
}
void PythonModule::set_object(const char* name, PyObject* object)
{
    if (object)
    {
        Py_INCREF(object);
        if (PyModule_AddObject(module(), name, object) < 0)
            PyErr_Print();
    }
}
void PythonModule::set_object(const char* name, Object* object)
{
    if (object)
    {
        if (PyModule_AddObject(module(), name, object->python_object()) < 0)
            PyErr_Print();
    }
}
void PythonModule::add_type(const char* name, PythonClass* type)
{
    set_object(name, (PyObject*)type->python_type());
}
PyObject* PythonModule::object(const char* name) const
{
    return PyObject_GetAttrString(_module, name);
}
const char* PythonModule::name() const
{
    return _module_def.m_name;
}
PyObject* PythonModule::module()
{
    if (!_module)
    {
        _module = PyModule_Create(&_module_def);
        if (!_module)
            PyErr_Print();
    }
    return _module;
}
PyObject* PythonModule::dict()
{
    return PyModule_GetDict(module());
}
