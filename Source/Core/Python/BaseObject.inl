namespace python
{
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
        
        // Holder will be the owner of our object, deleting it whenever it gets destroyed.
        PtrHolder<TClass>* holder = new PtrHolder<TClass>();
        TClass* obj = new (holder->ptr()) TClass(args...);
        
        PyTypeObject* type = TypeInfo<TClass>::info.py_type;
        if (type == nullptr)
        {
            PyErr_Format(PyExc_TypeError, "No python type found for type '%s'",
                typeid(TClass).name());
            return nullptr;
        }

        // Create a Python object which will act as the owner of obj
        PyObject* pyobj = incref(make_instance(type, holder).ptr());
        // Let the C++ object know of its owner
        obj->initialize_python(pyobj);

        return obj;
    }
}
