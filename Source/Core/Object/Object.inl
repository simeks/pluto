
template<typename T>
bool Object::is_a() const
{
    return is_a(T::static_class());
}

template<typename T>
void Object::set_attribute(const char* name, const T& attr)
{
    set_attribute(name, python::Object(python::to_python<T>(attr)));
}
template<int N>
void Object::set_attribute(const char* name, const char(&value)[N])
{
    set_attribute(name, PyUnicode_FromString(value));
}
template<typename T>
T Object::attribute(const char* name) const
{
    return python::from_python<T>(attribute(name));
}

template<typename T>
ObjectPtr<T>::ObjectPtr(T* ptr) : _ptr(ptr)
{
    assert(_ptr);
    _ptr->addref();
}
template<typename T>
ObjectPtr<T>::~ObjectPtr()
{
    assert(_ptr);
    _ptr->release();
}
template<typename T>
T* ObjectPtr<T>::operator->() const
{
    return _ptr;
}
template<typename T>
T* ObjectPtr<T>::ptr() const
{
    return _ptr;
}

template<typename T>
template<typename TOther>
ObjectPtr<T>::ObjectPtr(const ObjectPtr<TOther>& other) : _ptr(other._ptr)
{
    assert(_ptr);
    _ptr->addref();
}

template<typename T>
template<typename TOther>
ObjectPtr<T>& ObjectPtr<T>::operator=(const ObjectPtr<TOther>& other)
{
    assert(this != &other);
    assert(other._ptr);
    other._ptr->addref();
    _ptr->release();
    _ptr = other._ptr;
    return *this;
}
template<typename T>
template<typename TOther>
ObjectPtr<T>& ObjectPtr<T>::operator=(TOther* other)
{
    assert(other);
    other->addref();
    _ptr->release();
    _ptr = other;
    return *this;
}
template<typename T>
ObjectPtr<T> ObjectPtr<T>::attach(T* ptr)
{
    ObjectPtr<T> p(ptr);
    ptr->release();
    return p;
}
template<typename T, typename U>
bool operator!=(const ObjectPtr<T>& rhs, const ObjectPtr<U>& lhs)
{
    return rhs._ptr != lhs._ptr;
}


template<typename TClass, typename ... TArgs>
TClass* make_object(TArgs... args)
{
    static_assert(std::is_base_of<Object, TClass>::value, "make_object only works for types derived from Object");
        
    python::Class cls = TClass::static_class();

    // Holder will be the owner of our object, deleting it whenever it gets destroyed.
    python::PtrHolder<TClass>* holder = new python::PtrHolder<TClass>();
    TClass* obj = new (holder->ptr()) TClass(args...);

    // Create a Python object which will act as the owner of obj
    PyObject* pyobj = python::incref(python::make_instance((PyTypeObject*)cls.ptr(), holder).ptr());
    // Let the C++ object know of its owner
    obj->initialize_python(pyobj);

    return obj;
}
template<typename TClass, typename ... TArgs>
ObjectPtr<TClass> make_object_ptr(TArgs... args)
{
    return ObjectPtr<TClass>(make_object<TClass>(args...));
}
template<typename TClass>
TClass* object_cast(Object* object)
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

template<typename T>
PyObject* base_object_to_python(void const* val)
{
    static_assert(std::is_base_of<Object, T>::value, "Object needs to inherit Object");
    T* p = *((T**)val);
    return python::incref(p->ptr());
}
