namespace python
{
    INLINE Object::Object() : _obj(incref(Py_None))
    {
        // Default constructor initializes object as None
    }
    INLINE Object::Object(PyObject* obj, bool borrowed_ref)
        : _obj(borrowed_ref ? incref(obj) : obj)
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
    INLINE bool Object::is_none() const
    {
        return (_obj == Py_None);
    }
    INLINE bool Object::is_instance(const Object& type) const
    {
        assert(PyType_Check(type.ptr()));
        return PyObject_IsInstance(_obj, type.ptr()) == 1;
    }
    INLINE bool Object::is_instance(PyTypeObject* type) const
    {
        return PyObject_IsInstance(_obj, (PyObject*)type) == 1;
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

    template<typename T>
    INLINE void def(const Object& m, const char* name, const T& obj)
    {
        python::setattr(m, name, python::to_python(obj));
    }
}
