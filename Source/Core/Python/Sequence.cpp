#include "Common.h"

#include "PythonCommon.h"
#include "Sequence.h"

namespace python
{
    Sequence::Sequence()
    {
    }
    Sequence::Sequence(const python::Object& s) : python::Object(s)
    {
        assert(PySequence_Check(ptr()));
    }
    Sequence::~Sequence()
    {
    }
    size_t Sequence::size() const
    {
        return PySequence_Size(ptr());
    }
    python::Object Sequence::get(size_t idx) const
    {
        return PySequence_GetItem(ptr(), idx);
    }
    bool Sequence::valid() const
    {
        return !is_none();
    }
}

