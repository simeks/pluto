#include "Common.h"

#include "Object/Object.h"
#include "PythonCommon.h"
#include "Sequence.h"

Sequence::Sequence() : _s(nullptr)
{
}
Sequence::Sequence(PyObject* s) : _s(s)
{
    if (s)
    {
        assert(PySequence_Check(s));
        Py_XINCREF(_s);
    }
}
Sequence::~Sequence()
{
    Py_XDECREF(_s);
}
size_t Sequence::size() const
{
    return PySequence_Size(_s);
}
PyObject* Sequence::get(size_t idx) const
{
    return PySequence_GetItem(_s, idx);
}
bool Sequence::valid() const
{
    return _s != nullptr;
}
PyObject* Sequence::ptr() const
{
    return _s;
}

