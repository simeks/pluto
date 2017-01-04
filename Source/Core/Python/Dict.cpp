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
    if (has_key(key))
        return PyDict_GetItemString(_d, key);
    return nullptr;
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
int Dict::next(size_t* pos, PyObject** key, PyObject** value) const
{
    return PyDict_Next(_d, (Py_ssize_t*)pos, key, value);
}
bool Dict::valid() const
{
    return _d != nullptr;
}
Dict Dict::copy() const
{
    if (!_d)
        return Dict();

    Dict d(PyDict_Copy(_d));
    Py_XDECREF(d._d);
    return d;
}
PyObject* Dict::dict() const
{
    return _d;
}
Dict::Dict(const Dict& other)
{
    _d = other._d;
    Py_XINCREF(_d);
}
Dict& Dict::operator=(const Dict& other)
{
    _d = other._d;
    Py_XINCREF(_d);
    return *this;
}

