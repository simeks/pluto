namespace python
{
    namespace class_registry
    {
        INLINE Entry::Entry(const std::type_info& cpp_type) :
            cpp_type(cpp_type),
            py_type(nullptr),
            to_python(nullptr),
            from_python(nullptr)
        {
        }
    }

    template<typename T>
    const class_registry::Entry& ClassInfo<T>::info = class_registry::lookup(typeid(T));

    template<typename T>
    ValueHolder<T>::ValueHolder()
    {
    }

    template<typename T>
    Object make_class(const char* name)
    {
        return make_class(name, nullptr);
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