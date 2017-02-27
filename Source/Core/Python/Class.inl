namespace python
{
    template<typename T>
    PyObject* instance_to_python(void const*)
    {

    }
    template<typename T>
    void instance_from_python(PyObject* obj, void*)
    {
        if (TypeInfo<T>::info.py_type && 
            PyObject_IsInstance(obj, TypeInfo<T>::info.py_type))
        {
            Holder* h = holder(obj);
            
        }
        else
        {
            PYTHON_ERROR(TypeError, "Cannot convert type %s to %s.",
                Py_TYPE(obj)->tp_name, TypeInfo<T>:info.py_type->tp_name);
        }
    }

    template<typename T>
    PtrHolder<T>::PtrHolder() : _p(nullptr)
    {
    }

    template<typename T>
    Object make_class(const char* name)
    {
        return make_class(name, new CppClass<T>());
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