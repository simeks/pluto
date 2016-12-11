#ifndef __CORE_PYTHON_CONVERT_H__
#define __CORE_PYTHON_CONVERT_H__

#include "PythonWrapper.h"

#include <Core/Image/Vec3.h>

namespace python_convert
{
    template<typename T>
    T from_python(PyObject* obj);
    INLINE PyObject* from_python(PyObject* value) { return value; }

    template<typename T>
    PyObject* to_python(const T& value);

    INLINE PyObject* to_python(PyObject* value) { return value; }
}


#endif // __CORE_PYTHON_CONVERT_H__
