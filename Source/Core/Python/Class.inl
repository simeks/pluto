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
    UniquePtrHolder<T>::UniquePtrHolder()
    {
        /// Allocate memory but do not call any constructor
        _p = std::unique_ptr<T>((T*)::operator new(sizeof(T)));
    }
    template<typename T>
    UniquePtrHolder<T>::UniquePtrHolder(std::unique_ptr<T> ptr) :
        _p(std::move(ptr))
    {
    }
    template<typename T>
    UniquePtrHolder<T>::~UniquePtrHolder()
    {
    }

    template<typename T>
    void* UniquePtrHolder<T>::ptr()
    {
        return _p.get();
    }

    template<typename T>
    Holder* CppClass<T>::allocate()
    {
        return new PtrHolder<T>();
    }

    template<typename TClass, typename ... TArgs>
    INLINE void Class::def_init()
    {
        setattr(*this, "__init__", make_function(class_init<TClass, TArgs...>, "__init__"));
    }

    template<typename TClass>
    INLINE void Class::def_init_varargs()
    {
        setattr(*this, "__init__", make_varargs_function(class_init<TClass, const Tuple&>, 
            "__init__"));
    }

    template<typename TClass>
    INLINE void Class::def_init_varargs_keywords()
    {
        setattr(*this, "__init__", make_varargs_keywords_function(class_init<TClass, const Tuple&, const Dict&>, 
            "__init__"));
    }

    template<typename TClass, typename TReturn, typename ... TArgs>
    INLINE void Class::def(const char* name, TReturn(TClass::*meth)(TArgs...), const char* doc)
    {
        // Works the same as method a bound function (Function.h) but with _self set to nullptr
        setattr(*this, name, make_function((TClass*)nullptr, meth, name, doc));
    }

    template<typename TClass, typename TReturn>
    INLINE void Class::def_varargs(const char* name, TReturn(TClass::*meth)(const Tuple&), const char* doc)
    {
        setattr(*this, name, make_varargs_function((TClass*)nullptr, meth, name, doc));
    }

    template<typename TClass, typename TReturn>
    INLINE void Class::def_varargs_keywords(const char* name, TReturn(TClass::*meth)(const Tuple&, const Dict&), 
        const char* doc)
    {
        setattr(*this, name, make_varargs_keywords_function((TClass*)nullptr, meth, name, doc));
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
    template<typename T>
    PyObject* instance_unique_ptr_to_python(void const* val)
    {
        PyTypeObject* type = TypeInfo<T>::info.py_type;
        if (!type)
        {
            PyErr_Format(PyExc_TypeError, "PyTypeObject not found for type '%s'.",
                typeid(T).name());
            return nullptr;
        }

        UniquePtrHolder<T>* h = new UniquePtrHolder<T>(std::move(*((std::unique_ptr<T>*)val))); // Instance will delete this
        return incref(make_instance(type, h).ptr());
    }

    template<typename TClass, typename TBaseClass>
    Class make_class(const char* name, const char* doc)
    {
        static_assert(std::is_base_of<TBaseClass, TClass>::value, "TClass needs to derive from TBaseClass");

        if (TypeInfo<TClass>::info.py_type != nullptr)
        {
            // This type already have a python type
            return Class(Borrowed((PyObject*)TypeInfo<TClass>::info.py_type));
        }

        PyTypeObject* base_type = nullptr;
        if (!std::is_same<TClass, TBaseClass>::value)
        {
            // User have specified a base class
            base_type = TypeInfo<TBaseClass>::info.py_type;
            assert(base_type); // We assume that the base type has already been defined
        }

        Class cls = make_class(name, new CppClass<TClass>(), base_type, doc);

        type_registry::insert(typeid(TClass*),
            (PyTypeObject*)cls.ptr(),
            instance_ptr_to_python<TClass>,
            instance_ptr_from_python<TClass>);

        type_registry::insert(typeid(TClass),
            (PyTypeObject*)cls.ptr(),
            instance_value_to_python<TClass>,
            instance_value_from_python<TClass>);

        type_registry::insert(typeid(std::unique_ptr<TClass>),
            (PyTypeObject*)cls.ptr(),
            instance_unique_ptr_to_python<TClass>,
            nullptr // Not supported
        );

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

}