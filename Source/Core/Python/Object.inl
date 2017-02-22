namespace python
{
    INLINE Object::Object() : _obj(incref(Py_None))
    {
        // Default constructor initializes object as None
    }
    INLINE Object::Object(PyObject* obj) : _obj(incref(obj))
    {
    }
    INLINE Object::~Object()
    {
        assert(Py_REFCNT(_obj) > 0); // Detect dangling reference
        Py_DECREF(_obj);
    }

    INLINE Object::Object(const Object& other) : _obj(incref(other._obj))
    {
    }
    INLINE Object& Object::operator=(const Object& other)
    {
        Py_INCREF(other._obj);
        Py_DECREF(_obj);
        _obj = other._obj;
        return *this;
    }
    INLINE PyObject* Object::ptr() const
    {
        return _obj;
    }

    INLINE PyObject* incref(PyObject* obj)
    {
        Py_INCREF(obj);
        return obj;
    }
    INLINE PyObject* xincref(PyObject* obj)
    {
        Py_XINCREF(obj);
        return obj;
    }

    INLINE PyObject* decref(PyObject* obj)
    {
        Py_DECREF(obj);
        return obj;
    }
    INLINE PyObject* xdecref(PyObject* obj)
    {
        Py_XDECREF(obj);
        return obj;
    }

}
