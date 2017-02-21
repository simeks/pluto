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

    template<typename T>
    T incref(T obj)
    {
        Py_INCREF(obj.ptr());
        return obj;
    }
    template<typename T>
    T xincref(T obj)
    {
        Py_XINCREF(obj.ptr());
        return obj;
    }

    template<typename T>
    T decref(T obj)
    {
        Py_DECREF(obj.ptr());
        return obj;
    }
    template<typename T>
    T xdecref(T obj)
    {
        Py_XDECREF(obj.ptr());
        return obj;
    }

    template<typename T>
    T getattr(const Object& obj, const char* key)
    {
        return python::from_python<T>(getattr(obj, key));
    }
    template<typename T>
    T getattr(const Object& obj, const char* key, const T& default)
    {
        PyObject* attr = PyObject_GetAttrString(obj.ptr(), key);
        if (!attr)
        {
            PyErr_Clear();
            return default;
        }
        T ret = python::from_python<T>(attr);
        Py_DECREF(attr); // TODO: Can we avoid this redundant reference handling? 
        return ret;
    }
    template<int N>
    const char* getattr(const Object& obj, const char* key, const char(&default)[N])
    {
        PyObject* attr = PyObject_GetAttrString(obj.ptr(), key);
        if (!attr)
        {
            PyErr_Clear();
            return default;
        }
        const char* ret = python::from_python<const char*>(attr);
        Py_DECREF(attr); // TODO: Can we avoid this redundant reference handling? 
        return ret;
    }

    template<typename T>
    void setattr(const Object& obj, const char* key, const T& value)
    {
        setattr<Object>(obj, key, Object(python::to_python(value)));
    }
}
