#include "Common.h"

#include "BaseObject.h"


namespace python
{
    PYTHON_OBJECT_IMPL(BaseObject, "BaseObject")
    {
        cls;
    }

    BaseObject::BaseObject() : _obj(nullptr), _type(nullptr)
    {
    }
    BaseObject::~BaseObject()
    {
    }
    bool BaseObject::has_attribute(const char* name) const
    {
        assert(_obj);
        return PyObject_HasAttrString(_obj, name) == 1;
    }
    Object BaseObject::attribute(const char* name) const
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
    void BaseObject::set_attribute(const char* name, const Object& obj)
    {
        assert(_obj);
        if (PyObject_SetAttrString(_obj, name, obj.ptr()) != 0)
            PyErr_Print();
    }
    bool BaseObject::is_a(const python::Class& type) const
    {
        if (type.is_none())
            return false;

        if (PyObject_IsInstance(_obj, type.ptr()))
            return true;

        return false;
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

    BaseObject::BaseObject(const BaseObject& other) 
    {
        _type = other._type;

        PyObject* copy = _type->tp_new(_type, nullptr, nullptr);
        // Using PtrHolder<BaseObject> should suffice as all classes here should subclass BaseObject
        // However, this assumes PtrHolder is just a "holder" that does not perform any magic depending on type
        ((Instance*)copy)->holder = new PtrHolder<BaseObject>(this, true);

        PyObject* src_dict = PyObject_GetAttrString(other._obj, "__dict__");
        PyObject* dst_dict = PyObject_GetAttrString(copy, "__dict__");
        PyDict_Update(dst_dict, src_dict);
        Py_DECREF(src_dict);
        Py_DECREF(dst_dict);

        _obj = copy; // Object will allocated by the cloned class
    }
}
