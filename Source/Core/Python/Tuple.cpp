#include "Common.h"

#include "PythonCommon.h"
#include "Tuple.h"

namespace python
{
    Tuple::Tuple()
    {
    }
    Tuple::Tuple(size_t size) : python::Object(PyTuple_New(size))
    {
    }
    Tuple::Tuple(const python::Object& t) : python::Object(PySequence_Tuple(t.ptr()))
    {
    }
    size_t Tuple::size() const
    {
        return PyTuple_Size(ptr());
    }
    void Tuple::set(size_t idx, const python::Object& obj)
    {
        PyTuple_SetItem(ptr(), idx, incref(obj.ptr()));
    }
    python::Object Tuple::get(size_t idx) const
    {
        return python::Object(python::Borrowed(PyTuple_GetItem(ptr(), idx)));
    }
    bool Tuple::valid() const
    {
        return !is_none();
    }
}
