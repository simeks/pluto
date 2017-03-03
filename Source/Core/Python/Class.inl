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
    Holder* CppClass<T>::allocate()
    {
        return new PtrHolder<T>();
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
            PyErr_Format(PyExc_TypeError, "Cannot convert pointer type %s to %s.",
                Py_TYPE(obj)->tp_name, TypeInfo<T>::info.py_type->tp_name);
        }
    }
    template<typename T>
    PyObject* instance_value_to_python(void const* val)
    {
        PyTypeObject* type = TypeInfo<T>::info.py_type;
        if (!type)
        {
            PyErr_Format(PyExc_TypeError, "PyTypeObject not found for type '%s'.",
                typeid(T).name());
            return nullptr;
        }

        PtrHolder<T>* h = new PtrHolder<T>(); // Instance will delete this
        new (h->ptr()) T(*((T*)val));
        return incref(make_instance(type, h).ptr());
    }

    template<typename T>
    void instance_value_from_python(PyObject* obj, void* val)
    {
        if (TypeInfo<T>::info.py_type &&
            PyObject_IsInstance(obj, (PyObject*)TypeInfo<T>::info.py_type))
        {
            new (val) T(*((T*)holder(obj)->ptr()));
        }
        else
        {
            PyErr_Format(PyExc_TypeError, "Cannot convert type %s to %s.",
                Py_TYPE(obj)->tp_name, TypeInfo<T>::info.py_type->tp_name);
        }
    }
    template<typename TClass>
    Object make_class(const char* name, const char* doc)
    {
        if (TypeInfo<TClass>::info.py_type != nullptr)
        {
            // This type already have a python type
            return Object(Borrowed((PyObject*)TypeInfo<TClass>::info.py_type));
        }

        Object cls = make_class(name, new CppClass<TClass>(), doc);

        type_registry::insert(typeid(TClass*),
            (PyTypeObject*)cls.ptr(),
            instance_ptr_to_python<TClass>,
            instance_ptr_from_python<TClass>);

        // No by-value converters implemented
        type_registry::insert(typeid(TClass),
            (PyTypeObject*)cls.ptr(),
            instance_value_to_python<TClass>,
            instance_value_from_python<TClass>);

        return cls;
    }
    template<typename TClass>
    Object make_instance(TClass* value)
    {
        PyTypeObject* type = TypeInfo<TClass>::info.py_type;
        if (type == nullptr)
        {
            PyErr_Format(PyExc_TypeError, "No python type found for type '%s'",
                typeid(TClass).name());
            return None();
        }

        return make_instance(type, new PtrHolder<TClass>(value));
    }


    template<typename TClass, typename ... TArgs>
    void class_init(TClass* self, TArgs... args)
    {
        new (self) TClass(args...);
    }

    template<typename TClass, typename ... TArgs>
    INLINE void def_init(const Object& cls)
    {
        setattr(cls, "__init__", make_function(class_init<TClass, TArgs...>, "__init__"));
    }

    template<typename TClass>
    INLINE void def_init_varargs(const Object& cls)
    {
        setattr(cls, "__init__", make_varargs_function(class_init<TClass, const Tuple&>, "__init__"));
    }

    template<typename TClass>
    INLINE void def_init_varargs_keywords(const Object& cls)
    {
        setattr(cls, "__init__", make_varargs_keywords_function(class_init<TClass, const Tuple&, const Dict&>, "__init__"));
    }

    template<typename TClass, typename TReturn, typename ... TArgs>
    void def(const Object& cls, const char* name, TReturn(TClass::*meth)(TArgs...), const char* doc)
    {
        assert(cls.is_instance(&PyType_Type)); // Makes no sense calling this on a non-type
        // Works the same as method a bound function (Function.h) but with _self set to nullptr
        python::setattr(cls, name, make_function((TClass*)nullptr, meth, name, doc));
    }

    template<typename TClass, typename TReturn>
    INLINE void def_varargs(const Object& cls, const char* name, 
        TReturn(TClass::*meth)(const Tuple&), const char* doc)
    {
        assert(cls.is_instance(&PyType_Type)); // Makes no sense calling this on a non-type
        python::setattr(cls, name, make_varargs_function((TClass*)nullptr, meth, name, doc));
    }

    template<typename TClass, typename TReturn>
    INLINE void def_varargs_keywords(const Object& cls, const char* name,
        TReturn(TClass::*meth)(const Tuple&, const Dict&), const char* doc)
    {
        assert(cls.is_instance(&PyType_Type)); // Makes no sense calling this on a non-type
        python::setattr(cls, name, make_varargs_keywords_function((TClass*)nullptr, meth, name, doc));
    }
}