#ifndef __CORE_PYTHON_CONVERT_H__
#define __CORE_PYTHON_CONVERT_H__

#include "PythonWrapper.h"

#include <Core/Image/Vec3.h>

class Object;

namespace python_convert
{
    template<typename T>
    T from_python(PyObject* obj);

    template<>
    INLINE PyObject* from_python(PyObject* value) { if (value) return value; else Py_RETURN_NONE; }

    /// Converts the specified value and returns a new reference to the resulting PyObject
    template<typename T>
    PyObject* to_python(const T& value);

    INLINE PyObject* to_python(PyObject* value) { if (value) return value; else Py_RETURN_NONE; }

    template<int N>
    INLINE PyObject* to_python(const char(&value)[N])
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
}


#endif // __CORE_PYTHON_CONVERT_H__
