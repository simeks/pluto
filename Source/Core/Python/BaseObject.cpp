#include "Common.h"

#include "BaseObject.h"


namespace python
{
    BaseObject::BaseObject() : _obj(nullptr)
    {
    }
    BaseObject::~BaseObject()
    {
    }
    bool BaseObject::has_attr(const char* name)
    {
        assert(_obj);
        return PyObject_HasAttrString(_obj, name) == 1;
    }
    Object BaseObject::attr(const char* name)
    {
        assert(_obj);
        PyObject* attr = PyObject_GetAttrString(_obj, name);
        if (!attr)
        {
            PyErr_Print(); // TODO: Error handling
            return Object(Py_None); // TODO: Return None or throw exception?
        }
        return attr;
    }
    bool BaseObject::is_overridden(const char* name)
    {
        assert(_obj);
        PyObject* method = PyObject_GetAttrString(_obj, name);
        if (method)
        {
            if (PyMethod_Check(method))
            {
                // We can assume that the function is not overridden (at least not by python)
                //  if the function is of the type defined in Function.cpp.

                if (PyFunction_Check(PyMethod_Function(method)))
                    return true;
            }
        }
        return false;
    }
    bool BaseObject::is_overridden(const Object& method)
    {
        if (PyMethod_Check(method.ptr()))
        {
            // We can assume that the function is not overridden (at least not by python)
            //  if the function is of the type defined in Function.cpp.

            if (PyFunction_Check(PyMethod_Function(method.ptr())))
                return true;
        }
        
        return false;
    }
    void BaseObject::addref()
    {
        assert(_obj);
        Py_INCREF(_obj);
    }
    void BaseObject::release()
    {
        assert(_obj);
        Py_DECREF(_obj);
    }
    PyObject* BaseObject::ptr()
    {
        return _obj;
    }
    void BaseObject::initialize_python(PyObject* obj)
    {
        _obj = obj;
        _type = Py_TYPE(obj);
    }
}
