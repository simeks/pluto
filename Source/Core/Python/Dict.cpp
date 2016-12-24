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

bool Dict::has_key(const std::string& key) const
{
    return PyMapping_HasKeyString(_d, key.c_str()) != 0;
}

PyObject* Dict::get(const std::string& key) const
{
    return PyDict_GetItemString(_d, key.c_str());
}

void Dict::set(const std::string& key, PyObject* item)
{
    PyDict_SetItemString(_d, key.c_str(), item);
}
void Dict::set(const std::string& key, Object* item)
{
    set(key, item->python_object());
}
bool Dict::valid() const
{
    return _d != nullptr;
}
PyObject* Dict::dict() const
{
    return _d;
}