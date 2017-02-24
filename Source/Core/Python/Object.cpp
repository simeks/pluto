#include "Common.h"

#include "Object.h"
#include "Object/PythonClass.h"
#include "PythonCommon.h"


namespace python
{
    Object None() { return Object(); }
    Object True() { return Object(Py_True); }
    Object False() { return Object(Py_False); }

    Object incref(const Object& obj)
    {
        Py_INCREF(obj.ptr());
        return obj;
    }
    Object decref(const Object& obj)
    {
        Py_DECREF(obj.ptr());
        return obj;
    }

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
        return attr;
    }
    Object getattr(const Object& obj, const char* key, const Object& default)
    {
        PyObject* attr = PyObject_GetAttrString(obj.ptr(), key);
        if (!attr)
        {
            PyErr_Clear();
            return default;
        }
        return attr;
    }

    void setattr(const Object& obj, const char* key, const Object& value)
    {
        if (PyObject_SetAttrString(obj.ptr(), key, value.ptr()) != 0)
            PyErr_Print();
    }

    Object call(const Object& obj, const Tuple& args, const Dict& kw)
    {
        return PyObject_Call(obj.ptr(), args.ptr(), kw.is_none() ? nullptr : kw.ptr());
    }


    //Object call(const Object& obj, const Tuple& args, const Dict& kw)
    //{
    //    PyObject_Call(obj.ptr(), args.ptr(), args.ptr();)
    //}

    void def(const Object& m, const char* name, PythonClass* cls)
    {
        python::setattr(m, name, (PyObject*)cls->python_type());
    }

}