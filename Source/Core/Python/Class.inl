namespace python
{
    template<typename T>
    PtrHolder<T>::PtrHolder() : _own(true)
    {
        /// Allocate memory but do not call any constructor
        _p = (T*)::operator new(sizeof(T));
    }

    template<typename T>
    PtrHolder<T>::PtrHolder(T* ptr) : _p(ptr), _own(false)
    {
    }

    template<typename T>
    PtrHolder<T>::~PtrHolder()
    {
        if (_own) // Only delete pointer if we actually own it
            delete _p;
    }

    template<typename T>
    void* PtrHolder<T>::ptr()
    {
        return _p;
    }

    template<typename T>
    PyObject* instance_ptr_to_python(void const* val)
    {
        PyTypeObject* type = TypeInfo<T>::info.py_type;
        if (!type)
        {
            PyErr_Format(PyExc_TypeError, "PyTypeObject not found for type '%s'.",
                typeid(T).name());
            return nullptr;
        }

        PtrHolder<T>* h = new PtrHolder<T>(*((T**)val)); // Instance will delete this
        return incref(make_instance(type, h).ptr());
    }

    template<typename T>
    void instance_ptr_from_python(PyObject* obj, void* val)
    {
        if (TypeInfo<T>::info.py_type &&
            PyObject_IsInstance(obj, (PyObject*)TypeInfo<T>::info.py_type))
        {
            *((T**)val) = (T*)holder(obj)->ptr();
        }
        else
        {
            PyErr_Format(PyExc_TypeError, "Cannot convert type %s to %s.",
                Py_TYPE(obj)->tp_name, TypeInfo<T>::info.py_type->tp_name);
        }
    }
    template<typename T>
    Object make_class(const char* name)
    {
        Object cls = make_class(name, new CppClass<T>());

        type_registry::insert(typeid(T*),
            (PyTypeObject*)cls.ptr(),
            instance_ptr_to_python<T>,
            instance_ptr_from_python<T>);

        // No by-value converters implemented
        type_registry::insert(typeid(T),
            (PyTypeObject*)cls.ptr(),
            nullptr,
            nullptr);

        return cls;
    }

    template<typename T, typename ... TArgs>
    void class_init(T* self, TArgs... args)
    {
        new (self) T(args...);
    }

    template<typename T, typename ... TArgs>
    void def_init(const Object& cls)
    {
        setattr(cls, "__init__", make_function(class_init<T, TArgs...>, "__init__"));
    }

}