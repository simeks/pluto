namespace python
{
    template<typename T>
    PyObject* instance_to_python(void const*)
    {

    }
    template<typename T>
    void instance_from_python(PyObject* obj, void* val)
    {
        if (TypeInfo<T>::info.py_type && 
            PyObject_IsInstance(obj, (PyObject*)TypeInfo<T>::info.py_type))
        {
        }
        else
        {
            PyErr_Format(PyExc_TypeError, "Cannot convert type %s to %s.",
                Py_TYPE(obj)->tp_name, TypeInfo<T>:info.py_type->tp_name);
        }
    }
    template<typename T>
    Object make_class(const char* name)
    {
        Object cls = make_class(name, new CppClass<T>());

        type_registry::insert(typeid(T*),
            (PyTypeObject*)cls.ptr(),
            instance_to_python<T*>,
            instance_from_python<T*>);

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