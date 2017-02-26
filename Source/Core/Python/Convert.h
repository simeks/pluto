#ifndef __CORE_PYTHON_CONVERT_H__
#define __CORE_PYTHON_CONVERT_H__


#include <Core/Image/Vec3.h>
#include <Core/Python/Class.h>
#include <Core/Python/Object.h>
#include <Core/Python/Types.h>


namespace python
{
    /// @remark This does not affect the reference count of given object.
    template<typename T>
    T from_python(PyObject* obj);

    template<typename T>
    T from_python(const Object& obj);

    /// Attempts to convert the given type to a python object and returns a new reference to said object.
    template<typename T>
    PyObject* to_python(const T& value);

    template<int N>
    PyObject* to_python(const char(&value)[N])
    {
        return PyUnicode_FromString(value);
    }

    template<typename T>
    PyObject* to_python(const std::vector<T>& value)
    {
        PyObject* l = PyList_New(value.size());
        for (size_t i = 0; i < value.size(); ++i)
            PyList_SetItem(l, i, to_python(value[i]));
        return l;
    }


    template<>
    CORE_API Vec3d from_python<Vec3d>(PyObject*);
}

#include "Convert.inl"


#endif // __CORE_PYTHON_CONVERT_H__
