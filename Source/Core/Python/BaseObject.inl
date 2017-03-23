namespace python
{
    template<typename T>
    bool BaseObject::is_a() const
    {
        return is_a(T::static_class());
    }

    template<typename T>
    void BaseObject::set_attribute(const char* name, const T& attr)
    {
        set_attribute(name, python::Object(python::to_python<T>(attr)));
    }
    template<int N>
    void BaseObject::set_attribute(const char* name, const char(&value)[N])
    {
        set_attribute(name, PyUnicode_FromString(value));
    }
    template<typename T>
    T BaseObject::attribute(const char* name) const
    {
        return python::from_python<T>(attribute(name));
    }

    template<typename T>
    Ref<T>::Ref(T* ptr) : _ptr(ptr)
    {
        assert(_ptr);
    }
    template<typename T>
    Ref<T>::~Ref()
    {
        assert(_ptr);
        _ptr->release();
    }
    template<typename T>
    T* Ref<T>::operator->() const
    {
        return _ptr;
    }

    template<typename T>
    template<typename TOther>
    Ref<T>::Ref(const Ref<TOther>& other) : _ptr(other._ptr)
    {
        assert(_ptr);
        _ptr->addref();
    }

    template<typename T>
    template<typename TOther>
    Ref<T>& Ref<T>::operator=(const Ref<TOther>& other)
    {
        assert(this != &other);
        assert(other._ptr);
        other._ptr->addref();
        _ptr->release();
        _ptr = other._ptr;
        return *this;
    }



    template<typename TClass, typename ... TArgs>
    TClass* make_object(TArgs... args)
    {
        static_assert(std::is_base_of<python::BaseObject, TClass>::value, "make_object only works for types derived from python::BaseObject");
        
        Class cls = TClass::static_class();

        // Holder will be the owner of our object, deleting it whenever it gets destroyed.
        PtrHolder<TClass>* holder = new PtrHolder<TClass>();
        TClass* obj = new (holder->ptr()) TClass(args...);

        // Create a Python object which will act as the owner of obj
        PyObject* pyobj = incref(make_instance((PyTypeObject*)cls.ptr(), holder).ptr());
        // Let the C++ object know of its owner
        obj->initialize_python(pyobj);

        return obj;
    }
    template<typename TClass>
    TClass* object_cast(python::BaseObject* object)
    {
        if (!object || !object->is_a(TClass::static_class()))
            return nullptr;

        return (TClass*)object;
    }
    template<typename TClass>
    TClass* clone_object(TClass* object)
    {
        return (TClass*)object->clone();
    }
}
