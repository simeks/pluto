#include "Common.h"

#include "Object.h"
#include "PythonCommon.h"

namespace python
{
    bool hasattr(const Object& obj, const char* key)
    {
        return PyObject_HasAttrString(obj.ptr(), key) == 1;
    }

    Object getattr(const Object& obj, const char* key)
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
    Object getattr(const Object& obj, const char* key, const Object& default)
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

    void setattr(const Object& obj, const char* key, const Object& value)
    {
        if (PyObject_SetAttrString(obj.ptr(), key, value.ptr()) != 0)
            PyErr_Print();
    }
}