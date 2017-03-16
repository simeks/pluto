#include "Common.h"

#include "Object.h"
#include "Object/PythonClass.h"
#include "PythonCommon.h"


namespace python
{
    Object Object::operator()() const
    {
        PyObject* args = PyTuple_New(0);
        PyObject* ret = PyObject_Call(ptr(), args, nullptr);
        assert(ret);
        return Object(ret);

    }
    Object Object::operator()(const Tuple& args) const
    {
        PyObject* ret = PyObject_Call(ptr(), args.ptr(), nullptr);
        assert(ret);
        return Object(ret);
    }
    Object Object::operator()(const Tuple& args, const Dict& kw) const
    {
        PyObject* ret = PyObject_Call(ptr(), args.ptr(), kw.ptr());
        assert(ret);
        return Object(ret);

    }

    Object None() { return Object(); }
    Object True() { return Object(incref(Py_True)); }
    Object False() { return Object(incref(Py_False)); }

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

}