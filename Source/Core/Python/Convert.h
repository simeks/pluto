#ifndef __CORE_PYTHON_CONVERT_H__
#define __CORE_PYTHON_CONVERT_H__

#include "PythonWrapper.h"

#include <Core/Image/Vec3.h>

namespace python_convert
{
    template<typename T>
    T from_python(PyObject* obj);

    template<>
    INLINE PyObject* from_python(PyObject* value) { if (value) return value; else Py_RETURN_NONE; }

    template<typename T>
    PyObject* to_python(const T& value);

    INLINE PyObject* to_python(PyObject* value) { if (value) return value; else Py_RETURN_NONE; }
}


#endif // __CORE_PYTHON_CONVERT_H__
