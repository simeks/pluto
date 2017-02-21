#include "Common.h"

#include "Object.h"
#include "PythonCommon.h"

namespace python
{
    template<>
    CORE_API python::Object from_python(PyObject* value) { if (value) return python::Object(value); else Py_RETURN_NONE; }
    template<>
    CORE_API PyObject* to_python(const python::Object& value) { return python::incref(value.ptr()); }
}

namespace python
{
    bool hasattr(const Object& obj, const char* key)
    {
        return PyObject_HasAttrString(obj.ptr(), key) == 1;
    }

    template<>
    CORE_API Object getattr<Object>(const Object& obj, const char* key)
    {
        PyObject* attr = PyObject_GetAttrString(obj.ptr(), key);
        if (!attr)
        {
            PyErr_Print(); // TODO: Error handling
            return Object(Py_None); // TODO: Return None or throw exception?
        }
        Object ret(attr);
        Py_DECREF(attr); // TODO: Can we avoid this redundant reference handling? 
        return ret;
    }
    template<>
    CORE_API Object getattr<Object>(const Object& obj, const char* key, const Object& default)
    {
        PyObject* attr = PyObject_GetAttrString(obj.ptr(), key);
        if (!attr)
        {
            PyErr_Clear();
            return default;
        }
        Object ret(attr);
        Py_DECREF(attr); // TODO: Can we avoid this redundant reference handling? 
        return ret;
    }

    template<>
    CORE_API void setattr<Object>(const Object& obj, const char* key, const Object& value)
    {
        if (PyObject_SetAttrString(obj.ptr(), key, value.ptr()) != 0)
            PyErr_Print();
    }
}