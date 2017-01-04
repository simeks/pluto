#include "Common.h"

#include "Object/Object.h"
#include "PythonCommon.h"
#include "Dict.h"

Dict::Dict()
{
    _d = PyDict_New();
}
Dict::Dict(PyObject* d) : _d(d)
{
    Py_XINCREF(_d);
}
Dict::~Dict()
{
    Py_XDECREF(_d);
}

bool Dict::has_key(const char* key) const
{
    return PyMapping_HasKeyString(_d, key) != 0;
}

PyObject* Dict::get(const char* key) const
{
    return PyDict_GetItemString(_d, key);
}

void Dict::set(const char* key, PyObject* item)
{
    PyDict_SetItemString(_d, key, item);
}
void Dict::set(const char* key, Object* item)
{
    set(key, item->python_object());
}
void Dict::clear()
{
    PyDict_Clear(_d);
}
bool Dict::valid() const
{
    return _d != nullptr;
}
Dict Dict::copy() const
{
    Dict d(PyDict_Copy(_d));
    Py_XDECREF(d._d);
    return d;
}
PyObject* Dict::dict() const
{
    return _d;
}
