#include "Common.h"

#include "Object/Object.h"
#include "PythonCommon.h"
#include "Tuple.h"

Tuple::Tuple() : _t(nullptr)
{
}
Tuple::Tuple(size_t size)
{
    _t = PyTuple_New(size);
}
Tuple::Tuple(PyObject* t) : _t(t)
{
    assert(PyTuple_Check(t));
    Py_XINCREF(_t);
}
Tuple::~Tuple()
{
    Py_XDECREF(_t);
}
size_t Tuple::size() const
{
    return PyTuple_Size(_t);
}
void Tuple::set(size_t idx, PyObject* obj)
{
    PyTuple_SetItem(_t, idx, obj);
}
void Tuple::set(size_t idx, Object* obj)
{
    PyTuple_SetItem(_t, idx, obj->python_object());
}
PyObject* Tuple::get(size_t idx) const
{
    return PyTuple_GetItem(_t, idx);
}
bool Tuple::valid() const
{
    return _t != nullptr;
}
PyObject* Tuple::tuple() const
{
    return _t;
}
Tuple::Tuple(const Tuple& other)
{
    _t = other._t;
    Py_XINCREF(_t);
}
Tuple& Tuple::operator=(const Tuple& other)
{
    _t = other._t;
    Py_XINCREF(_t);
    return *this;
}

