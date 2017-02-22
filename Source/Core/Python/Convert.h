#ifndef __CORE_PYTHON_CONVERT_H__
#define __CORE_PYTHON_CONVERT_H__


#include <Core/Image/Vec3.h>
#include <Core/Python/Object.h>


namespace python
{
    template<typename T>
    T from_python(const python::Object& obj);

    template<typename T>
    python::Object to_python(const T& value);

    template<int N>
    python::Object to_python(const char(&value)[N])
    {
        return PyUnicode_FromString(value);
    }

    template<typename T>
    python::Object to_python(const std::vector<T>& value)
    {
        PyObject* l = PyList_New(value.size());
        for (size_t i = 0; i < value.size(); ++i)
            PyList_SetItem(l, i, to_python(value[i]));
        return l;
    }

}


#endif // __CORE_PYTHON_CONVERT_H__
