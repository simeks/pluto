namespace python
{

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